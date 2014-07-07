#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

#define FARLEFT   1
#define NEARLEFT  2
#define HEAD      3
#define NEARRIGHT 4
#define FARRIGHT  5

int phrasefork(monad * m, int whichbar) {
	list * phrase = m->command;
	list * bar = list_get_list(phrase, whichbar);
	list * execinbar = list_find_list(bar, "exec");
	
	list * tofork = list_new();
	list * todo = list_append_list(tofork);
	
	/* Copy the thing(s) to do in the bar first. */
	list_append_copy(todo, execinbar);
	list_drop(todo, 1);
	
	/* Then copy the rest of the phrase to do. */
	int i;
	list * phrasedo = list_append_list(todo);
	list_append_token(phrasedo, "phrase");
	for(i = 2; i <= phrase->length; i++) {
		list * copyme = list_get_list(phrase, i);
		if(!copyme) continue;
		if(i == whichbar) continue;
		list_append_copy(list_append_list(phrasedo), copyme);
	}
	
	monad_join(m, monad_spawn(m, tofork, 0));
	list_free(tofork);
	
	m->alive = 0;
	
	return 0;
}

int phrasebranch(monad * m, int p) {
	int i;
	int retval = 0;
	list * phrase = m->command;
	
	if(m->debug) {
		printf("Trying to branch to the ");
		if(p == NEARLEFT || p == NEARRIGHT) printf("near ");
		if(p == FARLEFT  || p == FARRIGHT)  printf("far ");
		if(p == NEARLEFT || p == FARLEFT) printf("left");
		if(p == NEARRIGHT || p == FARRIGHT) printf("right");
		if(p == HEAD) printf("head");
		printf(".\n");				
	}
	
	for(i = 2; i<=phrase->length; i++) {
		
		/* Find each bar */
		list * bar = list_get_list(m->command, i);
		if(!bar) continue;
		if(!list_get_token(bar, 1)) continue;
		if(strcmp(list_get_token(bar, 1), "bar")) continue;
		
		/* Find the placement flags, make sure it's LEFT branching */
		list * placement = list_find_list(bar, "placement");
		
		switch(p) {
			case NEARLEFT:
				if(!list_contains(placement, "left"))  continue;
				if(list_contains(placement, "far"))  continue;
				break;
			case FARLEFT:
				if(!list_contains(placement, "left"))  continue;
				if(list_contains(placement, "near"))  continue;
				break;
			case NEARRIGHT:
				if(!list_contains(placement, "right"))  continue;
				if(list_contains(placement, "far"))  continue;
				break;
			case FARRIGHT:
				if(!list_contains(placement, "right"))  continue;
				if(list_contains(placement, "near"))  continue;
				break;
			case HEAD:
				if(!list_contains(placement, "head")) continue;
				break;
		}
		
		phrasefork(m, i);
		retval = 1;
	}
	return retval;
}

/* This function executes the instruction, and returns 1 if it was possible to do so.
 * Otherwise it returns 0.
 */
int tranny_phrase_ops(monad * m, char * command) {
	
	/* (lit ...) */
	if(!strcmp(command, "phrase")) {
		if(!phrasebranch(m, FARLEFT)) {
			if(m->debug) {
				printf("I have done all the far left branching I can with this phrase.\n");
			}
		}
		
		if(!phrasebranch(m, NEARLEFT)) {
			if(m->debug) {
				printf("I have done all the near left branching I can with this phrase.\n");
			}
		}
		
		if(!phrasebranch(m, HEAD)) {
			if(m->debug) {
				printf("I seem to have already processed the head of this phrase.\n");
			}
		}
		
		if(!phrasebranch(m, NEARRIGHT)) {
			if(m->debug) {
				printf("I have done all the near right branching I can with this phrase.\n");
			}
		}
		
		if(!phrasebranch(m, FARRIGHT)) {
			if(m->debug) {
				printf("I have done all the far right branching I can with this phrase.\nEnd of phrase generation.\n");
			}
		}
		
		return 1;
	}

	return 0;
}

int tranny_phrase(monad * m, char * command) {
	
	if(tranny_phrase_ops(m, command)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

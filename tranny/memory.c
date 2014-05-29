#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>


/* This block of code begins the life of a flashbulb, and gives it a particular name, by appending one to the end of the list of
 * flashbulbs, naming it, and putting the current scope in there.
 */
void mem_begin(monad * m, char * memname) {
	list * memory = list_find_list(m->namespace, "flashbulb");
	if(!memory) {
		memory = list_append_list(m->namespace);
		list_append_token(memory, "memory");
	} else if(m->debug) {
		printf("Here are the current flashbulbs: ");
		list_prettyprinter(memory);
	}
	
	list * flashbulb = list_append_list(memory);
	list_append_token(flashbulb, memname);
	list * scope = list_append_list(flashbulb);
	list_append_token(scope, "scope");
	list_append_copy(scope, m->scopestack);
	
	if(m->debug) {
		printf("Here are the current memory regions: ");
		list_prettyprinter(memory);
	}
		
}

/* This block of code ends the life of a flashbulb with a particular name, by deleting the last one so called from the list of 
 * known flashbulbs.
 */
void mem_end(monad *m, char * memname) {
	int i;
	
	list * memory = list_find_list(m->namespace, "memory");
	if(!memory) return;
	
	for(i = memory->length; i; i--) {
		list * flashbulb = list_get_list(memory, i);
		if(!flashbulb) continue;
		
		if(!strcmp(list_get_token(flashbulb, 1), memname)) {
			list_drop(memory, i);
			return;
		}
	}
	return;
}

void recall(monad * m, char * memname) {
	list * flashbulb = 0;
	int i;
	
	list * memory = list_find_list(m->namespace, "memory");
	if(!memory) {
		m->alive = 0;
		return;
	}
	
	for(i = memory->length; i; i--) {
		list * fb = list_get_list(memory, i);
		if(!fb) continue;
		
		if(!strcmp(list_get_token(fb, 1), memname)) {
			flashbulb = fb;
			break;
		}
	}
	
	if(!flashbulb) {
		if(m->debug) printf("Couldn't find any flashbulb to recall from!\n");
		m->alive = 0;
		return;
	}
	
	list * todo = list_find_list(flashbulb, "todo");
	if(todo) {
		/* Update the stack */
		list * newstack = list_new();
		list_append_copy(newstack, todo);
		list_append_copy(newstack, m->stack);
		list_free(m->stack);
		m->stack = newstack;
		list_drop(m->stack, 1);
		if(m->debug) {
			printf("This will be the new stack:\n");
			list_prettyprinter(m->stack);
			printf("\n");
		}
	}
}	

void remember(monad * m, char * memname) {
	int i;
	list * flashbulb = 0;
	
	list * memory = list_find_list(m->namespace, "memory");
	if(!memory) return;
	
	for(i = memory->length; i; i--) {
		list * fb = list_get_list(memory, i);
		if(!fb) continue;
		
		if(!strcmp(list_get_token(fb, 1), memname)) {
			flashbulb = fb;
			break;
		}
	}
	
	if(!flashbulb) {
		if(m->debug) printf("Could not find a flashbulb; this monad will die.\n");
		m->alive = 0;
	}
	
	if(m->debug) {
		if(flashbulb) {
			printf("Chose this flashbulb:\n");
			list_prettyprinter(flashbulb);
			printf("\n");
		}
	}
	
	list * todo = list_find_list(m->command, "todo");
	if(todo) {
		list_append_copy(list_append_list(flashbulb), todo);
	}

	if(m->debug) {
		if(flashbulb) {
			printf("The flashbulb ended up looking like this:\n");
			list_prettyprinter(flashbulb);
			printf("\n");
		}
	}
}

int tranny_memory(monad * m, char * command) {
	
	char * memname = list_get_token(m->command, 2);
	
	/* These commands set memory flashbulbs up and down. */
	if(!strcmp(command, "kill-flashbulb")) {
		mem_end(m, memname);
		list_free(m->command);
		return 1;
	}
	if(!strcmp(command, "flashbulb")) {
		mem_begin(m, memname);
		list_free(m->command);
		return 1;
	}
	
	/* These commands save and recall things in the memory.
	 * (They do not need to be done in that order! Things may be speculatively recalled before the have been remembered) */
	if(!strcmp(command, "remember")) {
		remember(m, memname);
		list_free(m->command);
		return 1;
	}
	if(!strcmp(command, "recall")) {
		recall(m, memname);
		list_free(m->command);
		return 1;
	}
	
	return 0;
}

#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

void tranny_segments(monad * m) {
	
	/* A list of segments is really a list of function calls. Generate that list: */
	list * calls = list_new();
	
	int i;
	for(i = 2; i <= m->command->length; i++) {
		char * segment = list_get_token(m->command, i);
		if(!segment) continue;
		
		list * call = list_append_list(calls);
		
		list_append_token(call, "call");
		list_append_token(call, "Segment");
		list_append_token(call, segment);
	}
	
	/* Update the stack */
	list * newstack = list_new();
	list_append_copy(newstack, calls);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	if(m->debug) {
		printf("This will be the new stack:\n");
		list_prettyprinter(m->stack);
		printf("\n");
	}

	/* Tidy up */
	list_free(calls);
}

void tranny_forgive(monad * m) {
	list * rules = list_new();
	list * parent = list_append_list(rules);
	
	list * child = list_append_list(rules);
	list * brake = list_append_list(parent);
	list * todo = list_append_list(child);
	
	list_append_token(brake, "brake");
	
	list_drop(m->command, 1);
	list_append_copy(todo, m->command);
	
	monad_join(m, monad_spawn(m, rules, 0));
}

void tranny_constituent(monad * m, int adjunct) {
	/* We'll get the list of rules that need to be parsed */
	list * patterns = 0;
	char * rulename = list_get_token(m->command, 2);
	patterns = list_find_list(m->rules, rulename);
	
	list * flags = list_new();
	int i;
	for(i = 3; i <= m->command->length; i++) {
		char * f = list_get_token(m->command, i);
		if(f) list_append_token(flags, f);
	}
	
	/* If there is such a list, spawn the children. */
	monad * children = 0;
	if(patterns) {
		if(!(children = monad_spawn(m, patterns, flags))) {
			if(m->debug) {
				printf("Couldn't find any definitions for ");
				printf("%s with the ", list_get_token(m->command, 2));
				printf("right flags.\n");
			}
		}

	/* Otherwise, maybe print some debugging info. */
	} else {
		if(m->debug) {
			printf("Couldn't find any definitions for ");
			printf("%s at all.\n", list_get_token(m->command, 2));
		}
	}
	if(adjunct) {
		if(m->debug) {
			if(children) printf("Adjunct mode. Monads %d and up will run next.\n", children->id);
			if(children) printf("If they live, I'll die, and if they die, I'll live.\n");
			if(!children) printf("I'll carry on then (let's hope this works).\n");
		}
		m->adjunct = children;
	} else {
		if(m->debug) {
			if(children) printf("Constituent mode. Monads %d and up will run next.\n", children->id);
			printf("I'll die now.\n");
		}
		monad_join(m, children);
		m->alive = 0;
	}
	
	list_free(flags);
	return;
}

void tranny_fork(monad * m) {
	list_drop(m->command, 1);
	monad_join(m, monad_spawn(m, m->command, 0));
	m->alive = 0;
}

void tranny_fuzzy(monad * m) {
	list * rules = list_new();
	list_append_list(rules);
	list * child = list_append_list(rules);
	list * confidence = list_append_list(child);
	list * todo = list_append_list(child);
	
	list_append_token(confidence, "confidence");
	list_append_token(confidence, list_get_token(m->command, 2));
	
	list_drop(m->command, 1);
	list_drop(m->command, 1);
	list_append_copy(todo, m->command);
	
	monad_join(m, monad_spawn(m, rules, 0));
	m->alive = 0;
}

int tranny_exec_ops(monad * m, char * command) {
	if(!strcmp(command, "constituent") || !strcmp(command, "call")) {
		tranny_constituent(m, 0);
		return 1;
	}
	if(!strcmp(command, "adjunct")) {
		tranny_constituent(m, 1);
		return 1;
	}
	if(!strcmp(command, "fork")) {
		tranny_fork(m);
		return 1;
	}
	if(!strcmp(command, "fuzzy")) {
		tranny_fuzzy(m);
		return 1;
	}
	if(!strcmp(command, "segments")) {
		tranny_segments(m);
		return 1;
	}
	
	
	return 0;
}
	
/* This tries to execute the instruction, and if it was possible to do that:
 *   - frees some memory up 
 *   - returns 1 to say "Success"
 * If it was not possible to execute the instruction, returns 0.
 */
int tranny_exec(monad * m, char * command) {
	
	if(tranny_exec_ops(m, command)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

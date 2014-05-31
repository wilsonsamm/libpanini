#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

/* This function should return zero if ALL the tokens in f1 are also in f2,
 * and 1 otherwise. */
int flagstester(list * f1, list * f2) {
	int i;
	for(i = 1; i <= f1->length; i++) {
		char * f = list_get_token(f1, i);
		if(!list_contains(f2, f)) return 1;
	}
	return 0;
}

/* Call this function by passing the monad and a list of rules, and
 * then the necessary rules will get spawned. */
monad * exec_spawn(monad * m, list * rules, list * flags, int(reduce)(monad * m, list * rules)) {
	int i;
	monad * first = 0;
	
	list * p;
	
	if(m->debug && flags) {
		printf("Here is the list of flags that the rules to be ");
		printf("spawned must have: ");
		list_prettyprinter(flags);
		printf("\n");
	}
	
	for(i = 1; i <= rules->length; i++) {
		monad * child = 0;
		
		/* Test if we were given a list back */
		p = list_get_list(rules, i);
		if(!p) continue;

		/* Test if the list contains the right flags */
		list * tf = list_find_list(p, "flags");
		if(flags && flags->length) {
			if(!tf) continue;
			if(flagstester(flags, tf)) continue;
		}

		/* Test if the reduce function won't cut this rule off */
		if(reduce) {
			if(reduce(m, p)) continue;
		}

		/* Construct a new linked list of monads */
		if(!first) {
			child = monad_copy_one(m);
			first = child;
		} else {
			child = monad_copy_one(m);
			monad_join(first, child);
		}

		list_append_copy(child->stack, p);
		list_append_copy(child->stack, m->stack);

		if(m->debug) {
			printf("Monad %d spawned monad %d to execute: ", m->id, child->id);
			list_prettyprinter(p);
			printf("\n");
		}
	}
	
	/* Return the linked list of monads we have created */
	return first;
}

void tranny_segments(monad * m) {
	
	/* Base case: no more segments to do. */
	if(m->command->length == 1) return;
	
	/* A list of segments is really a list of function calls. Generate that list: */
	list * calls = list_new();
	
	char * segment = list_get_token(m->command, 2);
	if(!segment) return;
	
	list * call = list_append_list(calls);
	
	list_append_token(call, "call");
	list_append_token(call, "Segment");
	list_append_token(call, segment);
	
	list * segments = list_append_list(calls);
	list_append_copy(segments, m->command);
	list_drop(segments, 2);
	
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

void tranny_call(monad * m, int adjunct, int(reduce)(monad * m, list * l)) {
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
		if(!(children = exec_spawn(m, patterns, flags, reduce))) {
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

void tranny_fork(monad * m, int(reduce)(monad * m, list * l)) {
	list_drop(m->command, 1);
	monad_join(m, exec_spawn(m, m->command, 0, reduce));
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

int tranny_exec_ops(monad * m, char * command, int(reduce)(monad * m, list * l)) {
	if(!strcmp(command, "constituent") || !strcmp(command, "call")) {
		tranny_call(m, 0, reduce);
		return 1;
	}
	if(!strcmp(command, "adjunct")) {
		tranny_call(m, 1, reduce);
		return 1;
	}
	if(!strcmp(command, "fork")) {
		tranny_fork(m, reduce);
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
int tranny_exec(monad * m, char * command, int(reduce)(monad * m, list * l)) {
	
	if(tranny_exec_ops(m, command, reduce)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

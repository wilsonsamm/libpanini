#include "../monad/monad.h"
#include "tranny.h"
#include "panini.h"
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
monad * exec_spawn(monad * m, list * rules, list * flags, int * switches) {
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
		if(switches) {
			if(reduce(m, p, switches)) continue;
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

void panini_segments(monad * m, int generate) {
	
	/* Base case: no more segments to do. */
	if(m->command->length == 1) return;
	
	/* What is the next segment to process? */
	char * segment = list_get_token(m->command, 2);
	if(!segment) return;
	
	/* While parsing, we might need to append this to the (record) 
	 */
	list * record = 0;
	record = get_namespace(m, "record", 0);
	if(record && generate == 0) list_append_token(record, segment);
		
	/* While generating, we might need to check that this is indeed the
	 * segment we want to call. */
	if(record && generate != 0) {
		char * current = list_get_token(record,2);
		
		if(!current || strcmp(segment, list_get_token(record,2))) {
			if(m->debug) {
				printf("This monad has died because it's not the right segment\n");
				printf("(TODO: why is this test in place?)\n");
			}
			m->alive = 0;
			return;
		}
		
		list_drop(record, 2);
	}
	
	/* A list of segments is really a list of function calls. Generate
	 * the list where the first instruction is that function call and 
	 * the second instrction is the next load of segments, eg:
	 *   (segments x y z)
	 * will become:
	 *   (call Segment x) (segments y z)
	 */
	list * spawn = list_new();
	list * calls = list_append_list(spawn);
		
	list * call = list_append_list(calls);
	
	list_append_token(call, "call");
	list_append_token(call, "Segment");
	list_append_token(call, segment);
	
	list * segments = list_append_list(calls);
	list_append_copy(segments, m->command);
	list_drop(segments, 2);
	
	if(!generate && m->namespace && list_find_list(m->namespace, "edit")) {
	/* Also, try not calling the segment, but decreasing the edit 
	 * distance. */
		list * edit1 = list_append_list(spawn);
		list * edec1 = list_append_list(edit1);
		list_append_token(edec1, "decrement-edit-distance");
		if(m->command->length > 2) {
			list * eseg1 = list_append_list(edit1);
			list_append_copy(eseg1, m->command);
			list_drop(eseg1, 2);
			list_drop(eseg1, 2);
		}
	
	/* Also, try calling some random segment instead, and also
	 * decreasing the edit distance. */
		list * edit2 = list_append_list(spawn);
		list * edec2 = list_append_list(edit2);
		list_append_token(edec2, "decrement-edit-distance");
		
		list * ecal2 = list_append_list(edit2);
		list_append_token(ecal2, "call");
		list_append_token(ecal2, "Segment");
		
		list * eseg2 = list_append_list(edit2);
		list_append_copy(eseg2, m->command);
		list_drop(eseg2, 2);
	}
		
	/* Spawn this whole lot of stuff */
	monad * c = exec_spawn(m, spawn, 0, 0);
	monad_join(m,c);
	
	/* Tidy up */
	m->alive = 0;
	list_free(spawn);
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

void panini_call(monad * m, int adjunct, int * switches) {
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
		if(!(children = exec_spawn(m, patterns, flags, switches))) {
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

void panini_fork(monad * m, int * switches) {
	list_drop(m->command, 1);
	monad * n = exec_spawn(m, m->command, 0, switches);
	monad_join(m, n);
	m->alive = 0;
}

void panini_fuzzy(monad * m) {
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
	list_free(rules);
}


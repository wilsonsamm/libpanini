#include "monad.h"
#include <string.h>
#include <stdlib.h>

monad * monad_copy_one(monad * m) {
	
	static int identification = 1;
	
	monad * child = monad_new();
		
	child->id = identification++;

	child->stack = list_new();

	child->rules = m->rules;

	child->alive = m->alive;

	if(m->namespace) {
		child->namespace = list_new();
		list_append_copy(child->namespace, m->namespace);
	}
	
	if(m->scopestack) {
		child->scopestack = list_new();
		list_append_copy(child->scopestack, m->scopestack);
	}
	
	if(child->outtext) {
		free(child->outtext);
		child->outtext = 0;
	}
	if(m->outtext) child->outtext = strdup(m->outtext);

	child->intext = m->intext;
	child->index = m->index;

	child->trace = m->trace;
	child->confidence = m->confidence;
	
	child->brake = m->brake;
	//child->adjunct = m->adjunct;
	child->howtobind = m->howtobind;
	
	
	child->parent_id = m->id;
	return child;
}

/* This function should return zero if ALL the tokens in f1 are also in f2,
 * and 1 otherwise. */
int spawn_flagstester(list * f1, list * f2) {
	int i;
	for(i = 1; i <= f1->length; i++) {
		char * f = list_get_token(f1, i);
		if(!list_contains(f2, f)) return 1;
	}
	return 0;
}

/* Call this function by passing the monad and a list of rules, and
 * then the necessary rules will get spawned. */
monad * monad_spawn(monad * m, list * rules, list * flags) {
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
			if(spawn_flagstester(flags, tf)) continue;
		}

		/* Construct a new linked list of monads */
		if(!first) {
			child = monad_copy_one(m);
			first = child;
		} else {
			child = monad_copy_one(m);
			monad_join(first, child);
		}
				
		//child->alive = m->alive;

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

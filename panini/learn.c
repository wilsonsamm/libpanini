#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>
void add_record_to_df(monad * m) {
	list * record = get_namespace(m, "record", 1);
	list * df = get_namespace(m, "df", 1);
	list * instr = list_append_list(df);
	char * test1 = list_get_token(m->command, 2);
	char * test2 = list_get_token(df, 2);
	if(!test1) {
		m->alive = 0;
		fprintf(stderr, "Syntax error. (add-record-to-df ...) takes one argument\n");
		return;
	}
	if(!test2) {
		m->alive = 0;
//		fprintf(stderr, "Problem with (df ...) ns.\n");
		return;
	}
	if(strcmp(test1, test2)) {
		m->alive = 0;
		return;
	}
	list_append_token(instr, "segments");
	list_append_copy(instr, record);
	list_drop(instr, 2);
	return;
}

void add_tag_to_df(monad * m) {
	list * df = get_namespace(m, "df", 1);
	if(strcmp(list_get_token(m->command, 2), list_get_token(df, 2))) {
		m->alive = 0;
		return;
	}
	list * instr = list_append_list(df);
	list_append_token(instr, "tag");
	return;
}

void add_flags_to_df(monad * m) {
	list * df = get_namespace(m, "df", 1);
	list * instr = list_append_list(df);
	if(strcmp(list_get_token(m->command, 2), list_get_token(df, 2))) {
		m->alive = 0;
		return;
	}
	list_append_token(instr, "flags");
	list_append_copy(instr, m->command);
	list_drop(instr, 2);
	return;
}
	
void add_ns_to_df(monad * m) {
	char * def = list_get_token(m->command, 2);
	char * nsname = list_get_token(m->command, 3);

	list * df = get_namespace(m, "df", 1);
	char * dfname = list_get_token(df, 2);
	if(strcmp(def, dfname)) {
		m->alive = 0;
		return;
	}

	list * ns = get_namespace(m, nsname, 1);
	if(!ns) return;
	
	list * instr = list_append_list(df);
	list_append_token(instr, nsname);
	list * varlist = m->command;
	int i;
	for(i = 4; i <= varlist->length; i++) {
		char * varname = list_get_token(varlist, i);
		
		/* If the namespace contains a positive binding, something like
		 * (seme variable-name), then append it to the instruction */
		if(list_contains(ns, varname))
			list_append_token(instr, varname);
		
		/* If the namespace contains a negative binding, something like
		 * (seme -variablename), then append that to the instruction */
		if(list_contains_neg(ns, varname)) {
			char * neg = malloc(strlen(varname) + 2);
			strcpy(neg, "-");
			strcat(neg, varname);
			list_append_token(instr, neg);
			free(neg);
		}
		
		/* If the namespace contains a binding of the variable to a 
		 * value, something like (seme (variable-name example), then 
		 * append that to the instruction */
		list * var = 0;
		if((var = list_find_list(ns, varname)) \
		    && (var->length == 2) \
		    && !list_get_list(var, 2)) {
				list_append_copy(list_append_list(instr), var);
		}
	}
}

list * learn_namespace(monad * m, char * nsname) {
	list * varlist = list_find_list(m->command, nsname);
	if(!varlist) return 0;
	list * ns = get_namespace(m, nsname, 0);
	if(!ns) return 0;
	
	list * instr = list_new();
	list_append_token(instr, nsname);
	int i;
	
	for(i = 2; i <= varlist->length; i++) {
		char * varname = list_get_token(varlist, i);
		
		/* If the namespace contains a positive binding, something like
		 * (seme variable-name), then append it to the instruction */
		if(list_contains(ns, varname))
			list_append_token(instr, varname);
		
		/* If the namespace contains a negative binding, something like
		 * (seme -variablename), then append that to the instruction */
		if(list_contains_neg(ns, varname)) {
			char * neg = malloc(strlen(varname) + 2);
			strcpy(neg, "-");
			strcat(neg, varname);
			list_append_token(instr, neg);
			free(neg);
		}
		
		/* If the namespace contains a binding of the variable to a 
		 * value, something like (seme (variable-name example), then 
		 * append that to the instruction */
		list * var = 0;
		if((var = list_find_list(ns, varname)) \
		    && (var->length == 2) \
		    && !list_get_list(var, 2)) {
				list_append_copy(list_append_list(instr), var);
		}
	}
	return instr;
}

list * learn_into(monad * m) {
	list * into = list_new();
	int i = 1;

	for(i = 3; i <= m->command->length; i++) {
		list * inst = list_get_list(m->command, i);
		if(!inst) continue;
		char * instc = list_get_token(inst, 1);
		if(!instc) continue;
		if(strcmp(instc, "into")) continue;

		char * nextscope = list_get_token(inst, 2);
		if(!nextscope) {
			nextscope = evaluate(m, list_get_list(m->command, 2));
			if(!nextscope) continue;
		}

		list * scope = get_namespace(m, list_get_token(inst, 3), 0);
		if(!scope) continue;
		scope = list_find_list(scope, nextscope);

		list * linst = list_append_list(into);
		list_append_token(linst, "into");
		if(list_get_token(inst, 2))
			list_append_token(linst, list_get_token(inst, 2));
		if(list_get_list(inst, 2))
			list_append_copy(list_append_list(linst), list_get_list(inst, 2));

		int j;
		for(j = 3; j <= inst->length; j++) {
			char * varname = list_get_token(inst, j);
			list * tvar = 0;
			if((tvar = list_find_list(scope, varname)))
				list_append_copy(list_append_list(linst), tvar);
		}
	}
	return into;
}
			
void monad_learn_open(monad * m, int * switches) {
	
	if(m->debug) {
		printf("this monad %d has had debugging turned on.\n", m->id);
	}
	
	/* Check that this monad has not yet learned anything else */
	if(get_namespace(m, "df", 0)) {
		if(m->debug) {
			printf("this monad will die because it, or its ancestor has already learned something.\n");
		}
		m->alive = 0;
		return;
	}
	list * df = get_namespace(m, "df", 1);
	list_append_token(df, list_get_token(m->command, 2));
	get_namespace(m, "record", 1);
}

void learn_tag(monad * m) {
//	if(m->learned) {
//		m->alive = 0;
//		return;
//	}
	char out[1024];
	snprintf(out, 1024, "(tag %s %s)\n", list_get_token(m->command, 2), list_get_token(m->command, 3));
	
	if(!m->outtext) m->outtext = strdup("");
	int newlen = strlen(m->outtext) + strlen(out) + 1;
	m->outtext = realloc(m->outtext, newlen);
	strcat(m->outtext, out);
	m->brake++;
}

/* This function has been especially written so that semantic primes may be
 * automatically bootstrapped. This means, that suppose you come across some as
 * yet unknown word, in some input, then it will be bound to the (head ...)
 * variable, and then picked up by some (open ...) routine which will generate
 * the dictionary definition.
 */
void learn_bootstrap(monad * m) {

	char word[1024];

	/* Find the next word in the input, skipping whitespace and punctuation */
	int i;
	for(i = 0; i<1024; i++) {
		if(m->index + i > strlen(m->intext)) break;
		char ch = m->intext[m->index];
		if(ch == ' ') break;
		if(ch == '.') break;
		if(ch == ',') break;
		if(ch == '?') break;
		if(ch == '!') break;
		word[i] = ch;
		m->index++;
	}
	word[i] = '\0'; /* null-termination */

	list * rule = list_new();
	list * seme = list_append_list(rule);
	list_append_token(seme, "seme");
	list * head = list_append_list(seme);
	list_append_token(head, "head");
	list_append_token(head, word);

	m->brake++;
	monad_join(m, monad_spawn(m, rule, 0));
}

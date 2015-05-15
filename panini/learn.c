#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

void guess_segments(monad * m) {
	list * segs = list_find_list(m->rules, "Segment");
	if(!segs) {
		m->alive = 0;
		if(m->debug) {
			printf("I have killed monad %d because it was ", m->id);
			printf("guessing segments but\n");
			printf("there are no segments defined.\n");
			printf("(see tranny/learn.c)\n");
		}
		return;
	}
	list * exe = list_new();
	list * exec = list_append_list(exe);
	list_append_token(exec, "fork");
	
	int i ;
	for(i = 2; i <= segs->length; i++) {
		list * cseg = list_get_list(segs, i);
		list * segname = list_find_list(cseg, "flags");
		
		/* Never bother guessing "space" or "fullstop". */
		char * test = list_get_token(segname, 2);
		if(!strcmp(test, "space")) continue;
		if(!strcmp(test, "fullstop")) continue;
		
		list * forknode = list_append_list(exec);
		//list * brake = list_append_list(forknode);
		//list_append_token(debug,"brake");
		list * seg = list_append_list(forknode);
		
		list_append_token(seg, "segments");
		list_append_token(seg, list_get_token(segname, 2));
	}

	list_append_copy(exe, m->stack);
	list_free(m->stack);
	m->stack = exe;
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
	
	/* Turn on debugging if we need to */
	if(list_find_list(m->command, "debug")) {
		m->debug = 1;
	}
	
	if(m->debug) {
		printf("this monad%d  has had debugging turned on.\n", m->id);
	}
	/* Check that this monad has not yet learned anything else */
	if(m->learned) {
		if(m->debug) {
			printf("this monad will die because it, or it's ancestor has already learned something.\n");
		}
		m->alive = 0;
		return;
	}
	
	/* Check that the thing has not been closed. */
	char * symb = list_get_token(m->command, 2);
	list * deff = list_find_list(m->rules, symb);
	if(deff && list_contains(deff, "closed")) {
		m->alive = 0;
		return;
	}
	
	/* We can't learn anything if there is no namespace. In that case, 
	 * then, the monad will just die.
	 */
	if(!m->namespace) {
		m->alive = 0;
		return;
	}

	/* Make sure we record all the segments we guess for this. */
	list * record = list_find_list(m->namespace, "record");
	if(!record) {
		record = list_append_list(m->namespace);
		list_append_token(record, "record");
	}
	
	/* Guess a segment if we need to */
	list * segments = list_find_list(m->command, "segments");
	if(segments) {
		int i = atoi(list_get_token(segments, 2)) - 1;
		if(i > 0) {

			/* The first thing to do is to reduce the counter by one.
			 * (then we will guess one or zero segments). */
			char dec[7];
			snprintf(dec, 7, "%d", i);
			list_drop(segments, 2);
			list_append_token(segments, dec);
			
			list * exec = list_new();
			list * exe1 = list_append_list(exec);
			list * exe2 = list_append_list(exec);
			list * seg = list_append_list(exe1);
			list * cur = list_append_list(exe1);
			//list * brk2 = list_append_list(exe2);
			list * alt = list_append_list(exe2);
			list * brk1 = list_append_list(exe1);
			list_append_token(seg, "guess-segments");
			list_append_copy(cur, m->command);

			list_append_token(brk1, "brake");
			//list_append_token(brk2, "brake");
			
			/* Did we get enough segments? */
			list_append_copy(alt, m->command);
			list * enough = list_find_list(alt, "segments");
			if(enough) {
				list_drop(enough, 2);
				list_append_token(enough, "0");
			}
			
			monad * children = exec_spawn(m, exec, 0, switches);
			//monad * children = exec_spawn(m, exec, 0, 0);
			monad_join(m, children);
			list_free(exec);
			return;
		}
	}
	list * alos = list_find_list(m->command, "at-least-one-segment");
	if(alos) {
		list * record = list_find_list(m->namespace, "record");
		if(record->length == 1) {
			m->alive = 0;
			return;
		}
	}
	
	/* handle all the namespaces */
	list * nseme  = learn_namespace(m, "seme");
	list * nrection=learn_namespace(m, "rection");
	list * ntheta = learn_namespace(m, "theta");

	list * into = learn_into(m);
	
	/* Get the flags */
	list * flags = list_find_list(m->command, "flags");
	
	/* Now construct the thing starting with (df ...
	 * which can be compiled by pcomp. */
	list * df = list_new();
	list_append_token(df, "df");
	list_append_token(df, list_get_token(m->command, 2));
	
	list * segs = list_append_list(df);
	list_append_token(segs, "segments");
	list_append_copy(segs, list_find_list(m->namespace, "record"));
	list_drop(segs, 2);
	if(segments && segs->length == 1) {
		m->alive = 0;
		list_free(df);
		if(nseme)    list_free(nseme);
		if(nrection) list_free(nrection);
		if(ntheta)   list_free(ntheta);
		if(into)     list_free(into);  
		return;
	}

	list * tag = list_append_list(df);
	list_append_token(tag, "tag");
	
	if(nseme)    list_append_copy(list_append_list(df),nseme);
	if(nrection) list_append_copy(list_append_list(df),nrection);
	if(ntheta)   list_append_copy(list_append_list(df),ntheta);
	
	if(flags)    list_append_copy(list_append_list(df),flags);
	if(into)     list_append_copy(df, into);
	
	char * definition = list_tochar(df);
	if(!m->outtext) m->outtext = strdup("");

	char * new = malloc(strlen(m->outtext) + strlen(definition) + 4);
	strcpy(new, definition);
	strcat(new, "\n");
	strcat(new, m->outtext);
	strcat(new, "\n");

	free(definition);
	free(m->outtext);
	m->outtext = new;
	
	if(nseme)    list_free(nseme);
	if(nrection) list_free(nrection);
	if(ntheta)   list_free(ntheta);
	if(into)     list_free(into);
	list_free(df);
	
	m->learned = 1;
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

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
		//list * debug = list_append_list(forknode);
		//list_append_token(debug,"debug");
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
		
		/* If the namespace contains a positive binding, something like
		 * (seme -variable-name), then append that to the instruction */
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
	
void monad_learn_open(monad * m) {

	/* We'll increment the brake register. */
	m->brake++;
	
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
			char dec[20];
			snprintf(dec, 20, "%d", i);
			list_drop(segments, 2);
			list_append_token(segments, dec);
		
			list * exec = list_new();
			list * exe1 = list_append_list(exec);
			list * exe2 = list_append_list(exec);
			list * seg = list_append_list(exe1);
			list * cur = list_append_list(exe1);
			list * alt = list_append_list(exe2);
			list_append_token(seg, "guess-segments");
			list_append_copy(cur, m->command);
			
			/* Did we get enough segments? */
			list_append_copy(alt, m->command);
			list * enough = list_find_list(alt, "segments");
			if(enough) {
				list_drop(enough, 2);
				list_append_token(enough, "0");
			}
			//list_append_copy(exe1, m->stack);
			//list_append_copy(exe2, m->stack);
			//list_free(m->stack);
			
			monad * children = exec_spawn(m, exec, 0, generate_reduce);
			//monad * children = exec_spawn(m, exec, 0, 0);
			monad_join(m, children);
			m->alive = 0;
			return;
		}
	}
	
	/* handle all the namespaces */
	list * nseme  = learn_namespace(m, "seme");
	list * nrection=learn_namespace(m, "rection");
	list * ntheta = learn_namespace(m, "theta");
	
	/* Get the flags */
	list * flags = list_find_list(m->command, "flags");
	
	/* Now construct the thing starting with (df ...
	 * which can be compiled by tc. */
	list * df = list_new();
	list_append_token(df, "df");
	list_append_token(df, list_get_token(m->command, 2));
	
	list * segs = list_append_list(df);
	list_append_token(segs, "segments");
	list_append_copy(segs, list_find_list(m->namespace, "record"));
	list_drop(segs, 2);
	if(segments && segs->length == 1) {
		m->alive = 0;
		return;
	}
	
	if(nseme)    list_append_copy(list_append_list(df),nseme);
	if(nrection) list_append_copy(list_append_list(df),nrection);
	if(ntheta)   list_append_copy(list_append_list(df),ntheta);
	
	if(flags)    list_append_copy(list_append_list(df),flags);
	
	char * definition = list_tochar(df);
	m->outtext = realloc(m->outtext, strlen(m->outtext) + strlen(definition) + 3);
	strcat(m->outtext, definition);
	strcat(m->outtext, "\n");
	free(definition);
	
	list_remove(m->namespace, "record");
	
}

void learn_tag(monad * m) {
	char out[1024];
	snprintf(out, 1024, "(tag %s %s)\n", list_get_token(m->command, 2), list_get_token(m->command, 3));
	
	if(!m->outtext) m->outtext = strdup("");
	int newlen = strlen(m->outtext) + strlen(out) + 1;
	m->outtext = realloc(m->outtext, newlen);
	strcat(m->outtext, out);
}
	

int tranny_learn(monad * m, void * nothing) {
	if(!m->alive) return 0;
	
	/* Pop the next command */
	monad_popcom(m);

	/* If there was no next command, then we must be finished ... */
	if(!m->command) return 0;
	
	if(m->debug) {
		printf("Monad %d is executing: ", m->id);
		if(m->command) list_prettyprinter(m->command);
		printf("\n");
	}

	char * command = list_get_token(m->command, 1);
	
	/* Is the current command a set of miscellaneous ones that work the same in all interpreters? */
	if(tranny_misc(m, command)) return 1;
	
	/* Is the currect command a set of commands that handle the INTEXT register? */
	if(tranny_intext(m, command)) return 1;
	
	/* Is the command one of those that spawns other monads? */
	if(tranny_exec(m, command, 0, 0)) return 1;
	
	/* Is the command one of the ones deals with memory? */
	if(tranny_memory(m, command)) return 1;
	
	/* Is the command one of the ones that binds variables? */
	if(tranny_binders(m, 0)) return 1;
	
	/* */
	if(!strcmp(command, "guess-segments")) {
		guess_segments(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	if(!strcmp(command, "into")) {
		monad_parse_into(m, 0, 0);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "return")) {
		monad_parse_return(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "open")) {
		monad_learn_open(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "tag")) {
		learn_tag(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program LEARN.\n", command);

	m->alive = 0;
	return 0;
}

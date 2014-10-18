#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

void monad_learn_open(monad * m) {
	if(!m->namespace) {
		m->alive = 0;
		return;
	}
	if(m->outtext) {
		m->alive = 0;
		return;
	}
	list * rule = list_new();
	list_append_token(rule, "df");
	list_append_token(rule, list_get_token(m->command, 2));
	
	int i;
	
	/* (lit ... ) */
	list * lit = list_append_list(rule);
	list_append_token(lit, "lit");
	list_append_token(lit, m->intext + m->index);
	if(m->debug) {
		printf("\t(lit %s)",m->intext + m->index);
	}
	
	/* (seme ... ) */
	list * seme = list_append_list(rule);
	list * oseme = list_find_list(m->namespace, "seme");
	if(!oseme) {
		list_free(rule);
		m->alive = 0;
		return;
	}
	list_append_copy(seme, oseme);
	
	/* (remove ... ) */
	list * remove = list_find_list(m->command, "remove");
	if(remove) {
		int j;
		for(i = 2; i <= remove->length; i++) {
			char * var = list_get_token(remove, i);
			for(j = 2; j <= seme->length; j++) {
			
				if(list_get_token(seme, j))
					if(!strcmp(var,list_get_token(seme, j)))
						list_drop(seme, j);
			
				if(list_get_list(seme, j))
					if(!strcmp(var,list_get_token(list_get_list(seme, j), 1)))
						list_drop(seme, j);	
			}
		}
	}
	if(m->debug) {
		list_prettyprinter(seme);
	}
	
	/* (flags ... ) */
	list * flags = list_find_list(m->command, "flags");
	if((flags = list_find_list(m->command, "flags"))) {
		list_append_copy(list_append_list(rule), flags);
	}
	if(m->debug && flags) {
		list_prettyprinter(flags);
	}
	
	/* (rection ... ) */
	list * rection = list_find_list(m->command, "rection");
	if((rection = list_find_list(m->command, "rection"))) {
		list_append_copy(list_append_list(rule), rection);
	}
	if(m->debug && rection) {
		list_prettyprinter(rection);
	}
	
	/* (attest ... )
	 * What this does is "attests" the new rule iff the parse program ever executes it. Unattested rules are taken with a pinch of 
	 * salt while generating. */
	list * attest = list_new();
	list_append_token(attest, "attest");
	list_append_token(attest, list_get_token(m->command, 2));
	for(i = 3; i <= rule->length; i++) {
		list * instruction = list_get_list(rule, i);
		if(instruction) list_append_copy(list_append_list(attest), instruction);
	}
	list_append_copy(list_append_list(rule), attest);
	
	/* Print the definition out! */
	m->outtext = strlist(rule, strdup(""));
	if(m->debug) {
		printf("\n\n\n\n");
		list_prettyprinter(rule);
	}
	list_free(rule);
	
}

void guess_segments(monad * m) {
	int i;
	
	list * segs_done = list_find_list(m->namespace, "segs_done");
	if(!segs_done) {
		segs_done = list_append_list(m->namespace);
		list_append_token(segs_done, "segs_done");
	}
		
	list * segments = list_find_list(m->rules, "Segment");
	
	list * todo = list_new();
	list_append_token(todo, "fork");
	
	/* For each defined segment, try it out, append the name of the segment, and brake. And then try to guess more segments. */
	for(i = 1; i <= segments->length; i++) {
		list * seg = list_get_list(segments, i);
		if(!seg) continue;
		list * fl = list_find_list(seg, "flags");
		
		list * g = list_append_list(todo);
		
		list * brake = list_append_list(g);
		list_append_token(brake, "brake");
		
		list * call = list_append_list(g);
		list_append_token(call, "call");
		list_append_token(call, "Segment");
		list_append_token(call, list_get_token(fl, 2));
		
		list * addseg = list_append_list(g);
		list_append_token(addseg, "addseg");
		list_append_token(addseg, list_get_token(fl, 2));
		
		list * guess = list_append_list(g);
		list_append_token(guess, "guess-segments");
	}
	
	/* Update the stack */
	list * newstack = list_new();
	list_append_copy(list_append_list(newstack), todo);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	if(m->debug) {
		printf("This will be the new stack:\n");
		list_prettyprinter(m->stack);
		printf("\n");
	}
	list_free(todo);
	return;
}

/* This thing makes a dictionary definition that could (theoretically) always be added to the language. */
void makedef(monad * m) {
	int i;
	
	/* If the monad has no namespace, it might as well die, no? */
	if(!m->namespace) {
		m->alive = 0;
		return;
	}
	
	/* If there is something in outtext, then it should probably be kept. So we'll die here, so as not to overwrite it. */
	if(m->outtext) {
		m->alive = 0;
		return;
	}
	
	/* Begin constructing the definition. Create a new list, append the statement "df" and whatever you want to call the thing. */
	list * rule = list_new();
	list_append_token(rule, "df");
	list_append_token(rule, list_get_token(m->command, 2));

	
	/* (segments ... ) 
	 * This takes whatever segments, were added my the (addseg) instruction if this has been run. */
	list * seg = list_find_list(m->namespace, "segments");
	if(seg) list_append_copy(list_append_list(rule), seg);
	
	/* (seme ... ) */
	list * seme = list_append_list(rule);
	list * oseme = get_namespace(m, "seme", 0);
	if(!oseme) {
		list_free(rule);
		m->alive = 0;
		return;
	}
	list_append_copy(seme, oseme);
	
	
	/* (remove ... ) */
	list * remove = list_find_list(m->command, "remove");
	if(remove) {
		int j;
		for(i = 2; i <= remove->length; i++) {
			char * var = list_get_token(remove, i);
			for(j = 2; j <= seme->length; j++) {
			
				if(list_get_token(seme, j))
					if(!strcmp(var,list_get_token(seme, j)))
						list_drop(seme, j);
			
				if(list_get_list(seme, j))
					if(!strcmp(var,list_get_token(list_get_list(seme, j), 1)))
						list_drop(seme, j);	
			}
		}
	}
	if(m->debug) {
		list_prettyprinter(seme);
	}
	
	/* (flags ... ) */
	list * flags = list_find_list(m->command, "flags");
	if((flags = list_find_list(m->command, "flags"))) {
		list_append_copy(list_append_list(rule), flags);
	}
	if(m->debug && flags) {
		list_prettyprinter(flags);
	}
	
	/* (rection ... ) */
	list * rection = list_find_list(m->command, "rection");
	if((rection = list_find_list(m->command, "rection"))) {
		list_append_copy(list_append_list(rule), rection);
	}
	if(m->debug && rection) {
		list_prettyprinter(rection);
	}
	
	/* (attest ... )
	 * What this does is "attests" the new rule iff the parse program ever executes it. Unattested rules are taken with a pinch of 
	 * salt while generating. */
	list * attest = list_new();
	list_append_token(attest, "attest");
	list_append_token(attest, list_get_token(m->command, 2));
	for(i = 3; i <= rule->length; i++) {
		list * instruction = list_get_list(rule, i);
		if(instruction) list_append_copy(list_append_list(attest), instruction);
	}
	list_append_copy(list_append_list(rule), attest);
	
	/* Print the definition out! */
	m->outtext = strlist(rule, strdup(""));

	list_free(rule);
	
}

void addseg(monad * m) {
	list * segs_done = list_find_list(m->namespace, "segments");
	if(!segs_done) {
		segs_done = list_append_list(m->namespace);
		list_append_token(segs_done, "segments");
	}
	list_append_token(segs_done, list_get_token(m->command, 2));
	if(m->debug) {
		printf("Guessed %s. List of guessed segments:", list_get_token(m->command, 2));
		list_prettyprinter(segs_done);
	}
	return;
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
	if(!strcmp(command, "addseg")) {
		addseg(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	if(!strcmp(command, "makedef")) {
		makedef(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	if(!strcmp(command, "into")) {
		monad_parse_into(m, 0);
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
	if(!strcmp(command, "attest")) {
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	
	printf("No such command as %s in the program LEARN.\n", command);

	m->alive = 0;
	return 0;
}

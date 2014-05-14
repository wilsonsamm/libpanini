#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

char * concat(char * a, char * b) {
	char * c = malloc(strlen(a) + strlen(b) + 1);
	strcpy(c, a);
	strcpy(c + strlen(a), b);
	free(a);
	free(b);
	return c;
}

char * strlist(list * l, char * takeme) {
	int i = 0;
	takeme = concat(takeme, strdup("("));
	while(i < l->length) {
		if(!l->types) break;
		if(l->types[i] == TOKEN) {
			char * tmp = concat(takeme, strdup((char *)l->data[i]));
			takeme = concat(tmp, strdup(" "));
		}
		if(l->types[i] == LIST) {
			takeme = strlist(l->data[i], strdup(takeme));
		}
		i++;
	}
	char * temp = concat(takeme, strdup(")"));
	return temp;
}

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
		list_append_copy(list_append_list(rule), flags);
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
	
	if(tranny_misc(m, command)) return 1;
	
	if(!strcmp(command, "strict")) {
		monad_parse_strict(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "constituent")) {
		monad_parse_constituent(m, 0);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "rection")) {
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "sandhi")) {
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "concord")) {
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "lit")) {
		monad_parse_lit(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "space")) {
		monad_parse_space(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "seme")) {
		m->howtobind &= ~(CREATE | WRITE);
		bind_vars(m);
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
	if(!strcmp(command, "fullstop")) {
		monad_parse_fullstop(m);
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
	if(!strcmp(command, "forgive")) {
		monad_parse_forgive(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "fork")) {
		monad_parse_fork(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "adjunct")) {
		monad_parse_constituent(m, 1);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "fuzzy")) {
		monad_parse_fuzzy(m);
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
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "language")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "check")) {
		monad_parse_check(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "read-ahead")) {
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "clues")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program LEARN.\n", command);

	m->alive = 0;
	return 0;
}

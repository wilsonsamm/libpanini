#include "../monad/monad.h"
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
			takeme = concat(takeme, (char *)l->data[i]);
			takeme = concat(takeme, strdup(" "));
		}
		if(l->types[i] == LIST) {
			takeme = strlist(l->data[i], strdup(takeme));
		}
		i++;
	}
	takeme = concat(takeme, strdup(")"));
	return takeme;
}

void monad_attest_attest(monad * m, char * langname) {
	if(!m->debug) {
		//m->debug = 1;
	}
	if(!langname) return;
	if(m->outtext) free(m->outtext);
	
	/* Construct the rule that will be written to the attest file */
	list * rule = list_new();
	list_append_token(rule, "df");
	list_drop(m->command, 1);
	list_append_copy(rule, m->command);
	
	m->outtext = strlist(rule, strdup(""));
	
	return;
}	

int tranny_attest(monad * m, char * langname) {
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
	
	if(!strcmp(command, "strict")) {
		monad_parse_strict(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}	
	if(!strcmp(command, "block")) {
		monad_parse_block(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}	
	if(!strcmp(command, "constituent")) {
		monad_parse_constituent(m, 0);
		list_free(m->command);
		m->command = 0;
		m->alive = 0;
		return 0;
	}
	if(!strcmp(command, "capital")) {
		m->capital = 1;
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "nop")) {
		monad_parse_nop();
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "flags")) {
		monad_parse_nop();
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "concord")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "confidence")) {
		monad_parse_confidence(m);
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
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "sandhi")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "theta")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "rection")) {
		m->howtobind |= CREATE | WRITE;
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
	if(!strcmp(command, "brake")) {
		monad_parse_brake(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "unbrake")) {
		monad_parse_unbrake(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "debug")) {
		monad_parse_debug(m);
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
	if(!strcmp(command, "fuzzy")) {
		monad_parse_fuzzy(m);
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
		//m->brake++;
		return 1;
	}
	if(!strcmp(command, "open")) {
		m->alive = 0;
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "attest")) {
		monad_attest_attest(m, langname);
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
		monad_parse_readahead(m);
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
	
	printf("No such command as %s in the program ATTEST.\n", command);

	m->alive = 0;
	return 0;
}

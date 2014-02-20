#include "../monad/monad.h"
#include <string.h>

void monad_learn_open(monad * m, FILE * output) {
	if(!m->namespace) {
		m->alive = 0;
		return;
	}
	list * rule = list_new();
	list_append_token(rule, "df");
	list_append_token(rule, list_get_token(m->command, 2));
	
	int i;
	for(i = 3; i <= m->command->length; i++) {
		list * instruction = list_get_list(m->command, i);
		list_append_copy(list_append_list(rule), instruction);
	}
	
	/* (lit ... ) */
	list * lit = list_append_list(rule);
	list_append_token(lit, "lit");
	list_append_token(lit, m->intext);
	
	/* (seme ... ) */
	list * seme = list_append_list(rule);
	list * oseme = list_find_list(m->namespace, "seme");
	if(!oseme) {
		list_free(rule);
		m->alive = 0;
		return;
	}
	
	list_append_copy(seme, oseme);

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
	list_fprettyprinter(output, rule);
	fprintf(output, "\n");
	list_free(rule);
	
}	

int tranny_learn(monad * m, FILE * output) {
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
		monad_generate_strict(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "constituent")) {
		monad_parse_constituent(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "capital")) {
		m->capital = 1;
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
		monad_generate_lit(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "space")) {
		monad_generate_space(m);
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
	if(!strcmp(command, "confidence")) {
		monad_parse_confidence(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "into")) {
		monad_parse_into(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "fullstop")) {
		monad_generate_fullstop(m);
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
	if(!strcmp(command, "debug")) {
		monad_parse_debug(m);
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
	if(!strcmp(command, "forgive")) {
		monad_generate_forgive(m);
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
		monad_parse_constituent(m);
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
		monad_learn_open(m, output);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "attest")) {
		monad_parse_brake(m);
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
	
	printf("No such command as %s in the program LEARN.\n", command);

	m->alive = 0;
	return 0;
}

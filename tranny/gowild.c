#include "../monad/monad.h"
#include <stdlib.h>
#include <string.h>
void monad_gowild_adjunct(monad * m) {
	/* We'll get the list of rules that need to be parsed */
	list * patterns = 0;
	char * rulename = list_get_token(m->command, 2);
	patterns = list_find_list(m->rules, rulename);
		
	/* get a list of flags */
	list * flags = list_new();
	int i;
	for(i = 3; i <= m->command->length; i++) {
		char * f = list_get_token(m->command, i);
		if(f) list_append_token(flags, f);
	}
	
	/* If there is such a list, spawn the children. */
	if(patterns) {
		if(!monad_spawn(m, patterns, 0)) {
			if(m->debug) {
				printf("Couldn't find any definitions for ");
				printf("%s that can ", list_get_token(m->command, 2));
				printf("be executed.\n");
			}
		}
		return;

	/* Otherwise, maybe print some debugging info. */
	} else {
		if(m->debug) {
			printf("Couldn't find any definitions for ");
			printf("%s at all.\n", list_get_token(m->command, 2));
		}
		m->alive = 0;
		return;
	}

	return;
}
	

int tranny_gowild(monad * m, void * nothing) {
	if(!m->alive) return 0;

	monad_popcom(m);
	if(!m->command) return 0;
	
	if(m->debug) {
		printf("Monad %d is executing: ", m->id);
		if(m->command) list_prettyprinter(m->command);
		printf("\n");
	}

	char * command = list_get_token(m->command, 1);
	
	if(!strcmp(command, "constituent")) {
		monad_parse_constituent(m);
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
	if(!strcmp(command, "confidence")) {
		monad_parse_confidence(m);
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
	if(!strcmp(command, "theta")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
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
	if(!strcmp(command, "sandhi")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
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
	if(!strcmp(command, "strict")) {
		monad_generate_strict(m);
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
	if(!strcmp(command, "concord")) {
		m->howtobind |= CREATE | WRITE;
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
		monad_parse_nop(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "open")) {
		m->alive = 0;
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
	if(!strcmp(command, "adjunct")) {
		monad_parse_nop(m);
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
	if(!strcmp(command, "attest")) {
		monad_parse_brake(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program GOWILD.\n", command);

	m->alive = 0;
	return 0;
}

#include "../monad/monad.h"
#include "../list/list.h"
#include "tranny.h"
#include <stdlib.h>
#include <string.h>


void monad_generate_strict(monad * m) {
	/* Set the STRICT flag */
	m->howtobind |= STRICT;
	
	/* The command that's to be done strictly */
	list_drop(m->command, 1);
	list * todo = list_new();
	list_append_copy(todo, m->command);

	/* Update the stack */
	list * newstack = list_new();
	list_append_copy(list_append_list(newstack), todo);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	return;
}

int tranny_generate(monad * m, void * nothing) {
	if(!m->alive) return 0;
	
	/* Pop the next command */
	monad_popcom(m);
	if(!m->command) return 0;
	
	if(m->debug) {
		printf("Monad %d is executing: ", m->id);
		if(m->command) list_prettyprinter(m->command);
		printf("\n");
	}

	char * command = list_get_token(m->command, 1);
	
	/* Is the current command a set of miscellaneous ones that work the same in all interpreters? */
	if(tranny_misc(m, command)) return 1;
	
	/* Is the current command a set of miscellaneous ones that append something to the output? */
	if(tranny_outtext(m, command)) return 1;
	
	/* Is the command one of those that spawns other monads? */
	if(tranny_exec(m, command)) return 1;
	
	if(!strcmp(command, "strict")) {
		monad_generate_strict(m);
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
	if(!strcmp(command, "theta")) {
		
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
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
	if(!strcmp(command, "seme")) {
		//m->howtobind &= ~(CREATE | WRITE);
		m->howtobind |= WRITE;
		//m->howtobind &= ~(CREATE);
		//m->howtobind = 0;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "into")) {
		monad_parse_into(m, 1);
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
	if(!strcmp(command, "attest")) {
		//monad_parse_nop(m);
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
	if(!strcmp(command, "clues")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program GENERATE.\n", command);

	m->alive = 0;
	return 0;
}

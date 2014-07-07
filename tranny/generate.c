#include "../monad/monad.h"
#include "../list/list.h"
#include "tranny.h"
#include <stdlib.h>
#include <string.h>

void generate_record(monad * m) {
	list * r = list_find_list(m->namespace, "record"); 
	if(!r) return;
	
	if(strcmp(list_get_token(m->command, 2), list_get_token(r, 2))) {
		m->alive = 0;
	} else {
		list_drop(r, 2);
	}
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
	if(tranny_exec(m, command, generate_reduce)) return 1;
	
	/* Is the command one of the ones deals with memory? */
	if(tranny_memory(m, command)) return 1;

	/* Is the command one of the ones that generates rules? */
	if(tranny_phrase(m, command)) return 1;
	
	/* Is the command one of the ones that binds variables? */
	if(tranny_binders(m, 1)) return 1;
	
	if(!strcmp(command, "record")) {
		generate_record(m);
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
	if(!strcmp(command, "check")) {
		monad_parse_check(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program GENERATE.\n", command);

	m->alive = 0;
	return 0;
}

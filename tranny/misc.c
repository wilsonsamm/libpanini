#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>


/* This function executes the instruction, and returns 1 if it was possible to do so.
 * Otherwise it returns 0.
 */
int tranny_misc_ops(monad * m, char * command) {
	
	/* (debug) */
	if(!strcmp(command, "debug")) {
		m->debug = 1;
		return 1;
	}
	
	/* (confidence 12) */
	if(!strcmp(command, "confidence")) {
		m->confidence += atoi(list_get_token(m->command, 2));;
		return 1;
	}
	
	/* (brake) and (unbrake) */
	if(!strcmp(command, "brake")) {
		m->brake++;
		return 1;
	}
	
	if(!strcmp(command, "unbrake")) {
		m->brake--;
		return 1;
	}
	
	/* (sandhiblock) */
	if(!strcmp(command, "sandhiblock")) {
		list_remove(m->namespace, "sandhi");
		return 1;
	}
	
	/* (nop), (flags ...) */
	if(!strcmp(command, "flags")) return 1;
	if(!strcmp(command, "nop")) return 1;
	
	/* None of these matches; going to return 0. */
	return 0;
}

/* This tries to execute the instruction, and if it was possible to do that:
 *   - frees some memory up 
 *   - returns 1 to say "Success"
 * If it was not possible to execute the instruction, returns 0.
 */
int tranny_misc(monad * m, char * command) {
	
	if(tranny_misc_ops(m, command)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

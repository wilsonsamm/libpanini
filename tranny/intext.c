#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

/* This function is like strcmp except it only looks as far as (char*)y goes */
int partialstrcmp(char * x, char * y) {
	int i;
	
	for(i = 0; y[i]; i++) {
		if(!x[i]) return y[i];
		if(x[i] != y[i]) return x[i] - y[i];
	}
	return 0;
}

/* This function sees if a string matches the intext plus the offset. 
 * If it DOES match, then:
 *   if adv is non-zero, the offset is increased appropriately, so that the next string will be matched to the next substring.
 * 
 * If it DOES NOT match, then:
 *   if kill is non-zero, the monad is killed.
 */
int scan_intext(monad * m, char * segment, int kill, int adv) {
	
	/* Check we're not about to read past the end of the string */
	if(strlen(m->intext) < m->index) {
		if(m->debug) {
			printf("Kicked the bucket because the index is beyond the end of the intext.\n");
			printf("Index: %d, Intext length: %d\n", m->index, strlen(m->intext)); 
		}
		if(kill) m->alive = 0;
		return 0;
	}
	
	/* Remove the sandhi bindings. (These will not apply on the other side of the segment) */
	if(m->namespace) list_remove(m->namespace, "sandhi");
	
	/* See if the next segment matches the next section of the string */
	if(partialstrcmp(m->intext + m->index, segment)) {
		if(m->debug) {
			printf("%s does not match %s.\n", segment, m->intext + m->index);
		}
		if(kill) m->alive = 0;
		return 0;
	}
	
	if(adv) m->index += strlen(segment);
	
	return 1;
}

/* This function executes the instruction, and returns 1 if it was possible to do so.
 * Otherwise it returns 0.
 */
int tranny_intext_ops(monad * m, char * command) {
	
	/* (lit ...) */
	if(!strcmp(command, "lit")) {
		scan_intext(m, list_get_token(m->command, 2), 1, 1);
		return 1;
	}
	
	/* (read-ahead ...) */
	if(!strcmp(command, "read-ahead")) {
		scan_intext(m, list_get_token(m->command, 2), 1, 0);
		return 1;
	}
	
	/* (space) */
	if(!strcmp(command, "space")) {
		scan_intext(m, " ", 1, 1);
		return 1;
	}
	/* (fullstop) */
	if(!strcmp(command, "fullstop")) {
		scan_intext(m, ".", 1, 1);
		return 1;
	}
	/* None of these matches; going to return 0. */
	return 0;
}


/* This tries to execute the instruction, and if it was possible to do that:
 *   - frees some memory up 
 *   - returns 1 to say "Success"
 * If it was not possible to execute the instruction, returns 0.
 */
int tranny_intext(monad * m, char * command) {
	
	if(tranny_intext_ops(m, command)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

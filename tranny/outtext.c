#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>


/* This function simply appends a string to the monad's outtext.
 */
void append_to_outtext(monad * m, char * segment) {
	
	/* Remove the sandhi bindings. (These will not apply on the other side of the segment) */
	if(m->namespace) list_remove(m->namespace, "sandhi");
	
	/* If this is the first thing to append, we'll just stock it in; */
	if(!m->outtext) {
		m->outtext = strdup(segment);
		return;
	}
	
	/* Otherwise attach the two string to eachother to generate the new string, */
	char * newstring = malloc(strlen(m->outtext) + strlen(segment) + 1);
	strcpy(newstring, m->outtext);
	strcat(newstring, segment);
	
	/* and then free() the old string and put the new one in its place. */
	free(m->outtext);
	m->outtext = newstring;
	return;
}

void gen_open(monad * m) {
	/* Does it say (rel open) (open ...) ? */
	list * ns = get_namespace(m, "seme");
	
	if(!ns) {
		m->alive = 0;
		return;
	}
	
	list * rel = list_find_list(ns, "head");
	if(!rel) {
		if(m->debug) {
			printf("There is no binding for head in the seme namespace.\n");
		}
		m->alive = 0;
		return;
	}
	char * r = list_get_token(rel, 2);
	if(strcmp(r, "open")) {
		if(m->debug) {
			printf("It does not say (rel open) in the seme namespace.\n");
		}
		m->alive = 0;
		return;
	}
	list * open = list_find_list(ns, "open");
	if(!open) {
		if(m->debug) {
			printf("It says (head open) in the seme namespace,");
			printf("but there is no (open ...) binding.\n");
			printf("This is an ontological error and there is ");
			printf("nothing I can do about it.\n");
		}
		m->alive = 0;
		return;
	}
	char * word = list_get_token(open, 2);
	
	/* Output the word, surrounded by whitespace. */
	append_to_outtext(m, " ");
	append_to_outtext(m, word);
	append_to_outtext(m, " ");
	
	return;
}

/* This function executes the instruction, and returns 1 if it was possible to do so.
 * Otherwise it returns 0.
 */
int tranny_outtext_ops(monad * m, char * command) {
	
	/* (lit ...) */
	if(!strcmp(command, "lit")) {
		append_to_outtext(m, list_get_token(m->command, 2));
		return 1;
	}
	
	/* (read-ahead ...) */
	if(!strcmp(command, "read-ahead")) {
		m->alive = 0;
		return 1;
	}
	
	/* (space) */
	if(!strcmp(command, "space")) {
		append_to_outtext(m, " ");
		return 1;
	}
	/* (fullstop) */
	if(!strcmp(command, "fullstop")) {
		append_to_outtext(m, ".");
		return 1;
	}
	/* (open) */
	if(!strcmp(command, "open")) {
		gen_open(m);
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
int tranny_outtext(monad * m, char * command) {
	
	if(tranny_outtext_ops(m, command)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

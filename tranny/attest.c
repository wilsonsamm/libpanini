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
	
	if(tranny_misc(m, command)) return 1;
	
	/* Is the current command a set of miscellaneous ones that work the same in all interpreters? */
	if(tranny_misc(m, command)) return 1;
	
	/* Is the currect command a set of commands that handle the INTEXT register? */
	if(tranny_intext(m, command)) return 1;
	
	/* Is the command one of those that spawns other monads? */
	if(tranny_exec(m, command, 0)) return 1;
	
	/* Is the command one of the ones deals with memory? */
	if(tranny_memory(m, command)) return 1;
	
	/* Is the command one of the ones that binds variables? */
	if(tranny_binders(m, 0)) return 1;
	
	if(!strcmp(command, "attest")) {
		monad_attest_attest(m, langname);
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
	if(!strcmp(command, "check")) {
		monad_parse_check(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program ATTEST.\n", command);

	m->alive = 0;
	return 0;
}

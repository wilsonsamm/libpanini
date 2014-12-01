#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

void monad_parse_dec_edit(monad * m) {
	char str[15];
	
	/* Find the edit variable */
	list * edit = list_find_list(m->namespace, "edit");
	if(!edit) {
		m->alive = 0;
		return;
	}
	
	/* Get the number out of it. */
	int e = atoi(list_get_token(edit, 2));
	
	/* If the number has reached 0, then the edit distance is too great
	 * so then we'll just kill the monad. */
	if(!e) {
		m->alive = 0;
		return;
	}
	
	/* Otherwise, we'll stick that number minus 1 back into the edit
	 * variable. */
	list_drop(edit, 2);
	
	e--;
	sprintf(str, "%d", e);
	list_append_token(edit, str);
	return;
}

void into_spawner_head(monad * m) {
	
	list * ns = get_namespace(m, "seme", 1);
	if(!ns) {
		m->alive = 0;
		return;
	}
	list * spawn = list_new();
	
	list_drop(m->command, 1);
	list_drop(m->command, 1);
	
	if(m->debug) {
		printf("Going to look for some scope to enter among these:\n");
		list_prettyprinter(ns);
	}
	
	int i = 1;
	for(i = 1; i <= ns->length; i++) {
		list * potential = list_get_list(ns, i);
		if(!potential) continue;
		
		if(m->debug) {
			printf("\nConsidering this one:");
			list_prettyprinter(potential);
		}
		
		char * pname = list_get_token(potential, 1);
		if(!pname) continue;
		
		list * headv = list_find_list(potential, "head");
		if(!headv) {
			if(m->debug) {
				printf("\nNo (head) variable.");
			}
			continue;
		}
		
		char * head = list_get_token(headv, 2);
		if(strcmp(head, pname)) {
			if(m->debug) {
				printf("\nWrong (head) variable. (%s != %s)", head, pname);
			}
			continue;
		}
		
		list * c = list_append_list(spawn);
		list * into = list_append_list(c);
		
		list_append_token(into, "into");
		list_append_token(into, pname);
		list_append_copy(into, m->command);
	}

	if(m->debug) {
		printf("\nHere is what I've decided to spawn:\n");
		list_prettyprinter(spawn);
		printf("\n");
	}
	
	monad_join(m, monad_spawn(m, spawn, 0));
	list_free(spawn);
	
	return;
}	

/* */
void monad_parse_into(monad * m, int head, int create) {
	/* The name of the scope we need to enter */
	char * intowhat = list_get_token(m->command, 2);
	if(!intowhat) {
		char * e = list_get_token(list_get_list(m->command, 2), 1);
		// Look the scopename up in some namespace.
		if(!strcmp(e, "rection") || !strcmp(e, "concord") || !strcmp(e, "seme") || !strcmp(e, "theta"))
			intowhat = evaluate(m, list_get_list(m->command, 2));
		
		// Go into a scope that names itself
		else if(!strcmp(e, "head")) {
			if(!head) {
				intowhat = ".temporary_token";
			} else {
				into_spawner_head(m);
				m->alive = 0;
			}
		}
		
		else if(m->debug) {
			printf("I don't know how to evaluate which scope to enter. This could be a problem in\n");
			printf("void monad_parse_into(monad * m), which you can find in parse.c.\n");
		}
	}
		
	if(!intowhat) {
		if(m->debug)
			printf("Could not evaluate which scope to enter.\n");
		m->alive = 0;
		return;
	} 
	
	if(m->debug) {
		printf("The scope we are going to enter is called ");
		printf("%s.\n", intowhat);
	}
	if(!m->scopestack) m->scopestack = list_new();
	list_append_token(m->scopestack, intowhat);
	
	/* We might need to check that the scope actually exists. */
	if(!get_namespace(m, "seme", create)) {
		if(m->debug) {
			printf("This monad is going to die because it wants to ");
			printf("go into the scope %s, which doesn't ", intowhat);
			printf("exist.\n");
		}
		m->alive = 0;
	}
	
	/* The command to execute in that scope */
	list * toexecute = list_new();
	list_append_copy(toexecute, m->command);
	list_drop(toexecute, 1);
	list_drop(toexecute, 1);

	/* The (return) command */
	list * retcom = list_new();
	list_append_token(retcom, "return");

	/* Update the stack */
	list * newstack = list_new();
	list_append_copy(list_append_list(newstack), toexecute);
	list_append_copy(list_append_list(newstack), retcom);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	if(m->debug) {
		printf("This will be the new stack:\n");
		list_prettyprinter(m->stack);
		printf("\n");
	}

	/* Tidy up */
	list_free(retcom);
	list_free(toexecute);
}

void monad_parse_return(monad * m) {
	/* This function returns after an (into ... ) instruction by popping a scopename off the scopestack. 
	 * The problem is if we said (into (head) do something). (head) means "whatever scope names itself in the (head) variable. 
	 * In that case, while parsing, (into) goes into a scope called .temporary_token and that is renamed when we return. */
	list * seme = get_namespace(m, "seme", 0);
	list * rection = get_namespace(m, "rection", 0);
	list * theta = get_namespace(m, "theta", 0);
	
	char * head = 0;
	list * headv = 0;
	
	if(seme) headv = list_find_list(seme, "head");
	if(headv) head = list_get_token(headv, 2);
	
	if(seme && !strcmp(list_get_token(seme, 1), ".temporary_token")) {
		if(!head) {
			m->alive = 0;
			if(m->debug) {
				printf("Monad died while returning, because we needed to rename the scope we left ");
				printf("but there was nothing to rename it to. \n(I look this up in the (head) ");
				printf("variable in the seme namespace, but there was no such variable.\n");
			}
			return;
		}
		if(seme) list_rename(seme, head);
		if(rection) list_rename(rection, head);
		if(theta) list_rename(theta, head);
	}
	
	list_drop(m->scopestack, m->scopestack->length);
	
}

void monad_parse_strict(monad * m) {
	list * rules = list_new();
	list_append_list(rules);
	list * child = list_append_list(rules);
	list * todo = list_append_list(child);
	
	list_drop(m->command, 1);
	list_append_copy(todo, m->command);
	
	monad_join(m, monad_spawn(m, rules, 0));
	m->alive = 0;
}

void monad_parse_open(monad * m) {
	/* how long is the instring? */
	char word[1024];
	char ch;
	int i;
	
	m->brake++;
	
	/* Skip whitespace */
	while(m->intext[m->index] == ' ') m->index++;
	
	for(i = 0; i<1024; i++) {
		if(m->index + i > strlen(m->intext)) break;
		ch = m->intext[m->index];
		if(ch == ' ') break;
		word[i] = ch;
		m->index++;
	}
	word[i] = '\0'; /* null-termination */
	if(m->debug) printf("(open) found the word \"%s\".\n",word);
}

void monad_parse_check(monad * m) {
	list * checks = get_namespace(m, "checks", 1);
	char * check = list_get_token(m->command, 2);
	
	if(list_contains(checks, check)) {
		if(m->debug) {
			printf("This monad has kicked the bucket since \"%s\" was found ", check);
			printf("to already have been checked. ");
		}
		m->alive = 0;
		return;
	}
	
	list_append_token(checks, check);
	return;
}

void monad_parse_pushscope(monad * m) {
	if(!m->scopestack) m->scopestack = list_new();
	list_append_token(m->scopestack, list_get_token(m->command, 2));
}

int tranny_parse(monad * m, void * nothing) {
	if(!m->alive) return 0;
	
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
	
	/* Is the current command one that matches something against the input? */
	if(tranny_intext(m, command)) return 1;
	
	/* Is the command one of those that spawns other monads? */
	if(tranny_exec(m, command, parse_reduce, 0)) return 1;
	
	/* Is the command one of the ones deals with memory? */
	if(tranny_memory(m, command)) return 1;

	/* Is the command one of the ones that binds variables? */
	if(tranny_binders(m, 0)) return 1;

	/* Is the command one of the ones that generates rules? */
	if(tranny_phrase(m, command)) return 1;
	
	if(!strcmp(command, "into")) {
		monad_parse_into(m, 0, 1);
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
	if(!strcmp(command, "open")) {
		monad_parse_open(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "attest")) {
		//monad_parse_brake(m);
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
	if(!strcmp(command, "pushscope")) {
		monad_parse_pushscope(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "decrement-edit-distance")) {
		monad_parse_dec_edit(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "tag")) {
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program PARSE.\n", command);

	m->alive = 0;
	return 0;
}

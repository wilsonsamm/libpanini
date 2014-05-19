#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

/* Enter a scope
 * This function takes a list of names of scopes, and enters each subscope */
list * enter_scope(list * namespace, list * scopelist, int create, int debug) {
	int i;
	for(i = 1; i <= scopelist->length; i++) {
		char * scopename = list_get_token(scopelist, i);
		char * neg = malloc(strlen(scopename) + 2);
		neg[0] = '-';
		strcpy(neg+1, scopename);
		if(list_contains(namespace, neg)) {
			free(neg);
			if(debug) {
				printf("Could not create a new scope \"%s\" since the ", scopename);
				printf("scopename has already been negatively bound.\n");
			}
			return 0;
		}
		free(neg);
		if(list_find_list(namespace, scopename)) {
			namespace = list_find_list(namespace, list_get_token(scopelist, i));
		} else {
			if(!create) {
				if(debug) printf("Not going to create the scope called \"%s\".\n", scopename);
				return 0;
			}
			namespace = list_append_list(namespace);
			list_append_token(namespace, list_get_token(scopelist, i));
		}
		
		if(debug) {
			printf("Entering \"%s\" in ", scopename);
			list_prettyprinter(namespace);
			printf("\n");
		}
	}
	return namespace;
}

/* This function returns the place where the variables are bound. */
list * get_namespace(monad * m, char * nsname, int create) {
	list * namespace;
	
	if(!m->namespace)  m->namespace  = list_new();
	if(!m->scopestack) m->scopestack = list_new();
	
	if(!(namespace = list_find_list(m->namespace, nsname))) {
		namespace = list_append_list(m->namespace);
		list_append_token(namespace, nsname);
	}
	
	if(!strcmp(nsname, "language")) return namespace;
	if(!strcmp(nsname, "sandhi")) return namespace;
	
	return enter_scope(namespace, m->scopestack, create, m->debug);
	
	return namespace;
}

/* This binds variables in the namespace given by the first token in the
 * COMMAND register and in the scope as given in the scopestack.
 * Iff create is non-zero, then the scope may be created.
 * Iff write is nonzero, then the variable will be bound (and not just
 * checked for compatibility). */
void bind_vars(monad * m) {
	list * namespace = get_namespace(m, list_get_token(m->command, 1), m->howtobind&CREATE?1:0);
	
	if(!namespace) {
		m->alive = 0;
		m->howtobind = 0;
		return;
	}
	
	/* If the WRITE flag is off, then do not create any new variables */
	if(!(m->howtobind & WRITE)) {
		int i;
		for(i = 2; i <= m->command->length; i++) {
			list * var = list_get_list(m->command, i);
			if(!var) continue;
			char * varname = list_get_token(var, 1);
			char * neg = negative(varname);
			if(list_find_list(namespace, varname)) continue;
			if(list_contains(namespace, neg)) continue;
			
			if(m->debug) {
				printf("The WRITE flag is off.\n");
				printf("There is no variable %s in the ", varname);
				printf("relevant namespace so I'm not going to ");
				printf("create it either.\n");
			}
			list_drop(m->command, i);
			
			free(neg);
		}
	}
	
	/* If the BLOCK flag is on, then delete all the specified variables. 
	 * */
	if((m->howtobind & BLOCK)) {
		int i;
		for(i = 2; i <= m->command->length; i++) {
			char * varname;
			list * var = list_get_list(m->command, i);
			if(!var) varname = list_get_token(m->command, i);
			else varname = list_get_token(var, 1);
			if(!varname) continue;

			/* What is the token's negative counterpart? */
			char * neg = malloc(strlen(varname) + 2);
			if(varname[0] == '-') {
			strcpy(neg, varname + 1);
			} else {
				neg[0] = '-';
				strcpy(neg + 1, varname);
			}
			if(m->debug) {
				printf("The relevant namespace is:\n");
				list_prettyprinter(namespace);
			}
			list_remove(namespace, varname);
			list_remove(namespace, neg);
			free(neg);
		}
		if(m->debug) {
			printf("After removing the variables, the relevant namespace is:\n");
			list_prettyprinter(namespace);
		}
		m->howtobind = 0;
		return;
	}
	
	/* If the STRICT flag is on, then make sure all the variables are 
	 * already bound. */
	if((m->howtobind & STRICT)) {
		int i;
		for(i = 2; i <= m->command->length; i++) {
			char * varname;
			list * var = list_get_list(m->command, i);
			if(!var) varname = list_get_token(m->command, i);
			else varname = list_get_token(var, 1);
			if(!varname) continue;

			/* What is the token's negative counterpart? */
			char * neg = malloc(strlen(varname) + 2);
			if(varname[0] == '-') {
			strcpy(neg, varname + 1);
			} else {
				neg[0] = '-';
				strcpy(neg + 1, varname);
			}
			if(m->debug) {
				printf("The relevant namespace is:\n");
				list_prettyprinter(namespace);
			}
			if((!list_find_list(namespace, varname)) && \
			   (!list_contains(namespace, varname)) && \
			   (!list_contains(namespace, neg))) m->alive = 0;
			free(neg);
			if(m->alive == 0) {
				if(m->debug) {
					printf("The STRICT flag is on.\n");
					printf("The variable %s is not bound in ", varname);
					printf("the relevant namespace, so this monad ");
					printf("will kick the bucket.\n");
				}
				m->howtobind = 0;
				return;
			}
		}
	}
	
	list_drop(m->command, 1);
	if(m->debug) {
		printf("These variables will be bound: \n");
		list_prettyprinter(m->command);
		printf("\nHere is the scopestack: \n");
		list_prettyprinter(m->scopestack);
		printf("\nThis is the relevant scope: \n");
		list_prettyprinter(namespace);
		printf("\n");
	}
	
	if(bind(m->command,namespace)) {
		m->alive = 0;
		if(m->debug) {
			printf("This monad has kicked the bucket due to some");
			printf("thing not binding.\n");
		}
	}
	
	if(m->debug) {
		printf("Here is the namespace as it is now:\n");
		list_prettyprinter(m->namespace);
		printf("\n");
	}
	
	m->howtobind = 0;
}

int tranny_howtobind_ops(monad * m) {

	char * command = list_get_token(m->command, 1);
	
	if(m->debug) 
		printf("I am trying to interpret this as a HOWTOBIND operation.\n");
	
	if(!strcmp(command, "strict")) {
		m->howtobind |= STRICT;
		list_drop(m->command, 1);
		return 1;
	}
	
	if(!strcmp(command, "block")) {
		m->howtobind |= BLOCK;
		list_drop(m->command, 1);
		return 1;
	}
	
	if(m->debug) 
		printf("This is not a HOWTOBIND operation.\n");
		
	return 0;
}

int tranny_binders_ops(monad * m, int gen) {

	char * command = list_get_token(m->command, 1);
	
	if(gen & !strcmp(command, "seme")) m->howtobind |= WRITE;
	else m->howtobind |= CREATE | WRITE;
	
	if(!strcmp(command, "language") || \
	   !strcmp(command, "rection")  || \
	   !strcmp(command, "theta")    || \
	   !strcmp(command, "clues")    || \
	   !strcmp(command, "sandhi")) {
		   m->howtobind |= CREATE | WRITE;
		   bind_vars(m);
		   return 1;
	}
	
	if(!strcmp(command, "seme")) {
		m->howtobind |= WRITE;
		if(!gen) m->howtobind |= CREATE;	
		bind_vars(m);
		return 1;
	}
	
	return 0;
}

/* This tries to execute the instruction, and if it was possible to do that:
 *   - frees some memory up 
 *   - returns 1 to say "Success"
 * If it was not possible to execute the instruction, returns 0.
 */
int tranny_binders(monad * m, int gen) {
	
	m->howtobind = 0;
	
	while(tranny_howtobind_ops(m));
	
	if(tranny_binders_ops(m, gen)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

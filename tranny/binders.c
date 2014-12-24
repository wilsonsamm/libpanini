#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

int create; // May we create a scope?
int write;	// May we write variables?

/* Enter a scope
 * This function takes a list of names of scopes, and enters each subscope */
list * enter_scope(list * namespace, list * scopelist, int create, int debug) {
	int i;
	for(i = 1; i <= scopelist->length; i++) {
		char * scopename = list_get_token(scopelist, i);
		
		if(list_contains_neg(namespace, scopename)) {
			if(debug) {
				printf("Could not create a new scope \"%s\" since the ", scopename);
				printf("scopename has already been negatively bound.\n");
			}
			return 0;
		}
		
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

/* This function returns the place where the variables are bound. 
 * If the integer "create" is zero, then if the namespace does not exist, then 
 * the function will return zero. Otherwise, the function will create the
 * namespace if it does not exist, and then return it.
 */
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
	list * namespace = get_namespace(m, list_get_token(m->command, 1), create);
	
	if(!namespace) {
		m->alive = 0;
		return;
	}
	
	/* If the WRITE flag is off, then do not create any new variables */
	if(!write) {
		int i;
		for(i = 2; i <= m->command->length; i++) {
			list * var = list_get_list(m->command, i);
			if(!var) continue;
			char * varname = list_get_token(var, 1);
			if(list_find_list(namespace, varname)) continue;
			if(list_contains_neg(namespace, varname)) continue;
			
			if(m->debug) {
				printf("The WRITE flag is off.\n");
				printf("There is no variable %s in the ", varname);
				printf("relevant namespace so I'm not going to ");
				printf("create it either.\n");
			}
			list_drop(m->command, i);
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
}

/* This function checks that it is possible to bind a list of variables in a namespace.
 * returns 1 for possible,
 * returns 0 for impossible.
 */
int check_vars(list * namespace, list * vars) {
	
	char varname[32];
	char varneg[32];
	char value[32];
	value[0] = '\0';
	
	int i;
	for(i = 2; i <= vars->length; i++) {

		list * var = list_get_list(vars, i);
		if(var) {
			strcpy(varname, list_get_token(var, 1));
			strcpy(value, list_get_token(var, 2));
		}
		
		char * t = list_get_token(vars, i);
		if(t) strcpy(varname, t);
		
		if(varname[0] != '-') {
				strcpy(varneg, "-");
				strcat(varneg, varname);
		} else {
			strcpy(varneg, varname+1);
		}
		
		if(list_contains(namespace, varneg)) return 0;
		
		if(strlen(value)) {
			list * cvar = list_find_list(namespace, varname);
			if(!cvar) continue;
			
			char * cval = list_get_token(cvar, 2);
			if(!cval) continue;
			
			if(strcmp(cval, value)) return 0;
		}
			
	}
	return 1;
}

int tranny_binders_ops(monad * m, int gen) {

	char * command = list_get_token(m->command, 1);

	write = 0;
	create = 0;
	
	if(gen & !strcmp(command, "seme")) write = 1;
	else {
		write = 1;
		create = 1;
	}
	
	if(!strcmp(command, "language") || \
	   !strcmp(command, "rection")  || \
	   !strcmp(command, "theta")    || \
	   !strcmp(command, "clues")    || \
	   !strcmp(command, "sandhi")) {
		   write = 1;
		   create = 1;
		   bind_vars(m);
		   return 1;
	}
	
	if(!strcmp(command, "seme")) {
		write = 1;
		if(!gen) create = 1;	
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
	
	if(tranny_binders_ops(m, gen)) {
		list_free(m->command);
		m->command = 0;
		return 1;
	} else {
		return 0;
	}
}

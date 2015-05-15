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

	/* Unscoped namespaces */
	if(!strcmp(nsname, "language")) return monadcow_copy(m, COW_LANG);
	if(!strcmp(nsname, "sandhi"))   return monadcow_copy(m, COW_SANDHI);
	if(!strcmp(nsname, "check"))    return monadcow_copy(m, COW_CHECK);

	/* Scoped namespaces */
	list * scopestack = monadcow_get(m, COW_SCOPE);
	
	list * namespace = 0;
	if(!strcmp(nsname, "rection"))  namespace = monadcow_copy(m, COW_RECTION);
	if(!strcmp(nsname, "seme"))     namespace = monadcow_copy(m, COW_SEME);
	if(!strcmp(nsname, "theta"))    namespace = monadcow_copy(m, COW_THETA);
	if(namespace) return enter_scope(namespace, scopestack, create, m->debug);
	
	return namespace;
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

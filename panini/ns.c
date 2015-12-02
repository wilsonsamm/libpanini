#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

#define MAXLENGTH 32

int create; // May we create a scope?
int write;	// May we write variables?

/* Enter a scope
 * This function takes a list of names of scopes, and enters each subscope */
list * enter_scope(list * namespace, list * scopelist, int create, int debug) {
	int i;
	for(i = 2; i <= scopelist->length; i++) {
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

list * getns_unscoped(monad * m, char * nsname, int create) {
	if(!m->namespace) {
		m->namespace = list_new();
	}
	list * ns = list_find_list(m->namespace, nsname);
	if(!ns && create) {
		ns = list_append_list(m->namespace);
		list_append_token(ns, nsname);
	}
	return ns;
}

/* This function returns the place where the variables are bound. 
 * If the integer "create" is zero, then if the namespace does not exist, then 
 * the function will return zero. Otherwise, the function will create the
 * namespace if it does not exist, and then return it.
 */
list * get_namespace(monad * m, char * nsname, int create) {

	/* Unscoped namespaces */
	if(!strcmp(nsname, "scopestack")) return getns_unscoped(m, "scopestack", create);
	if(!strcmp(nsname, "language"))   return getns_unscoped(m, "language", create);
	if(!strcmp(nsname, "sandhi"))     return getns_unscoped(m, "sandhi", create);
	if(!strcmp(nsname, "check"))      return getns_unscoped(m, "check", create);
	if(!strcmp(nsname, "record"))     return getns_unscoped(m, "record", create);
	if(!strcmp(nsname, "df"))         return getns_unscoped(m, "df", create);

	/* Scoped namespaces */
	list * scopestack = get_namespace(m, "scopestack", 1);
	
	list * namespace = 0;
	
	if(!strcmp(nsname, "rection"))  namespace = getns_unscoped(m, "rection", create);
	if(!strcmp(nsname, "seme"))     namespace = getns_unscoped(m, "seme", create);
	if(!strcmp(nsname, "theta"))    namespace = getns_unscoped(m, "theta", create);
	if(namespace) return enter_scope(namespace, scopestack, create, m->debug);
	
	return namespace;
}

/* This function checks that it is possible to bind a list of variables in a namespace.
 * returns 1 for possible,
 * returns 0 for impossible.
 */
int check_vars(list * namespace, list * vars) {
	
	char varname[MAXLENGTH];
	char varneg[MAXLENGTH];
	char value[MAXLENGTH];
	value[0] = '\0';
	
	int i;
	for(i = 2; i <= vars->length; i++) {

		list * var = list_get_list(vars, i);
		if(var) {
			strncpy(varname, list_get_token(var, 1), MAXLENGTH);
			strncpy(value, list_get_token(var, 2), MAXLENGTH);
		}
		
		char * t = list_get_token(vars, i);
		if(t) strcpy(varname, t);
		
		if(varname[0] != '-') {
				strncpy(varneg, "-", MAXLENGTH);
				strncat(varneg, varname, MAXLENGTH - 1);
		} else {
			strncpy(varneg, varname+1, MAXLENGTH - 1);
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

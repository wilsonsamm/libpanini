#include "../list/list.h"
#include "../monad/monad.h"
#include "tranny.h"
#include <stdlib.h>
#include <string.h>

/* What does this file do? Well, it "binds" variables in a given 
 * namespace. Variables may be:
 *  - boolean
 *  - token
 * And the way these work is: 
 *     A boolean variable is either Yes or No, One or Zero, Positive or 
 * Negative. These are represented as varname or -varname.
 *     A token variable can have any value, say "Charles" or "went" or
 * "42.7". These are represented as (varname Charles), (varname went) or
 * (varname 42.7).
 * 		If a variable name is assigned to be a negative boolean, it 
 * implies that is is not already bound to be a token variable.
 */
/* One of the important datatypes in Panini is the variable. A variable has
 * three features:
 *  - The key,      or a name by which we can refer to a variable.
 *  - The polarity, either positive or negative.
 *  - The value,    some string which the variable is "bound to". This may not
 *                  contain any whitespace.
 * The key is always bound either positive or negative, but the value is
 * optional. If the variable has a value, that implies it is also positive.
 * The variable is immutable: once it has been bound, it is stuck with that
 * value forever (with some exceptions).
 *
 * Variables are kept in namespaces. Some namespaces are scoped, and others are
 * unscoped. If a namespace is scoped, that means you may name a scope, thereby
 * creating it, and bind variables in it. Variables in different scopes are
 * independant of each other, even if they have the same name.
 */

int checkvars(monad * m, list * varlist, int writeprotect) {

	/* Get the name of the namespace. */
	char * nsname = list_get_token(varlist, 1);

	/* Get the namespace where the variables should be bound */
	list * ns = get_namespace(m, nsname, !writeprotect);

	/* For each of the variables in the list of variables to bind, */
	int i;
	for(i = 2; i <= varlist->length; i++) {

		/* These variables will represent the variable as it looks in the
		 * list of variables to bind. */
		char * key = 0;
		char * value = 0;
		int polarity = 0; // A value of 1 will mean negative, 0 = positive.

		/* These variables will represent the variable as it looks in the
		 * namespace. */
		char * ovalue = 0;
		int opolarity = 0; // A value of 1 will mean negative, 0 = positive.
		int found = 0; // If the variable is already bound, we'll put 1 here
		
		/* Find the key of the variable to bind */
		if(list_get_token(varlist, i)) {
			key = list_get_token(varlist, i);
			if(key[0] == '-') {
				key = key + 1;
				polarity = 1;
			}
		} else {
			key = list_get_token(list_get_list(varlist, i), 1);
			value = list_get_token(list_get_list(varlist, i), 2);
		}

		/* If the namespace isn't there, then the variable can not be bound. */
		if(!ns) return 1;

		/* Find the variable in the namespace */
		if(list_contains(ns, key)) {
			found = 1;
		}
		if(list_contains_neg(ns, key)) {
			found = 1;
			opolarity = 1;
		}
		if(list_find_list(ns, key)) {
			ovalue = list_get_token(list_find_list(ns, key), 2);
			found = 1;
		}

		/* Check if the variable can be bound */
		if(found && value && ovalue && strcmp(value, ovalue)) {
			if(m->debug) {
				printf("%s cannot be bound 1.\n", key);
			}
			return i;
		}
		if(found && polarity != opolarity) {
			if(m->debug) {
				printf("%s cannot be bound 2.\n", key);
			}
			return i;
		}

		/* Unless write protection is turned on, or the variable already exists
		 * and already has a value, we can go ahead in append the variable to
		 * the namespace. */
		if(!writeprotect && value && ((!ovalue && found) || (!found))) {
			list * newv = list_append_list(ns);
			list_append_token(newv, key);
			list_append_token(newv, value);
			ovalue = value;
			found = 1;
		}

		/* Unless write protection is turned on, or the variable already exists
		 * and has no value yet, we can go ahead in append the variable to the
		 * namespace. */
		if(!writeprotect && !found && !value && !polarity) {
			list_append_token(ns, key);
		}
		if(!writeprotect && !found && !value && polarity) {
			char * n = malloc(strlen(key) + 2);
			strcpy(n, "-");
			strcat(n, key);
			list_append_token(ns, n);
			free(n);
		}
	}
	return 0;
}

void negative(char * token, char * neg) {
	if(token[0] == '-') {
		strcpy(neg, token + 1);
	} else {
		neg[0] = '-';
		strcpy(neg + 1, token);
	}
	return;
}

/* This function returns the value held in a token variable. (or the 
 * NULL pointer if it's not bound or is boolean) */
char * eval(list * namespace, char * varname) {
	list * var = list_find_list(namespace, varname);
	if(!var) return 0;
	
	return list_get_token(var, 2);
}
	
void speculate(monad * m, char * namespace, char * varname) {

	char neg[32];
	
	/* This function speculates what a variable might be by looking at the program.
	 * (But first the instruction which is currently executing must be 
	 * saved on the stack) */
	list * newstack = list_new();
	list_append_copy(list_append_list(newstack), m->command);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	
	/* This bit will collect the rections from the rules and put them in
	 * a format suitable for passing to monad_parse_spawn, i. e.:
	 * (((rection (gender masculine)))
	 *  ((rection (gender feminine)))
	 *  ((rection (gender neuter)))) */
	list * rections = list_new();
	list * searchme = 0;
	list * values = list_new();
	
	searchme = m->rules;
	
	int i;
	for(i = 1; i<= searchme->length; i++) {
		int j;
		list * rules = list_get_list(searchme, i);
		if(!rules) continue;
		
		for(j = 1; j <= rules->length; j++) {
			list * rule = list_get_list(rules, j);
			if(!rule) continue;
			
			list * rection = list_find_list(rule, namespace);
			if(!rection) continue;
			
			list * gotcha = list_find_list(rection, varname);
			if(gotcha) {
				char * value = list_get_token(gotcha, 2);
				if(list_contains(values, value)) continue;
				
				list_append_token(values, value);
				list * r = list_append_list(list_append_list(rections));
				list_append_token(r, namespace);
				list_append_copy(list_append_list(r), gotcha);
				continue;
			}
			
			negative(varname, neg);
			
			if(list_contains(rection, neg)) {
				if(list_contains(values, neg)) {
					continue;
				}
				
				list_append_token(values, neg);
				list * r = list_append_list(list_append_list(rections));
				list_append_token(r, namespace);
			}
		}
	}
	
	/* Now let's spawn all the monads to bind the variables. */
	monad_join(m, monad_spawn(m, rections, 0));
	list_free(rections);
	list_free(values);
}

char * evaluate(monad * m, list * e) {
	/* The namespace */
	char neg[32];
	char * ns = list_get_token(e,1);
	
	list * namespace = get_namespace(m, ns, 1);
	if(!namespace) return 0;
	
	/* The variable's name (which can itself be evaluated) */
	char * varname = list_get_token(e,2); 
	if(!varname) return 0;
	negative(varname, neg);
	if(namespace && list_contains(namespace, neg)) {
		return 0;
	}
	
	/* See if this variable can just be looked up in the namespace. */
	char * retval = 0;
	retval = eval(namespace, varname);
	if(retval) return retval;
	
	speculate(m, ns, varname);
	return 0;
}

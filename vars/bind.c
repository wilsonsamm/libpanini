#include "../list/list.h"
#include "../monad/monad.h"
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

int bind_bool(list * l, char * token) {
	
	/* What is the token's negative counterpart? */
	char * neg = malloc(strlen(token) + 2);

	if(token[0] == '-') {
		strcpy(neg, token + 1);
	} else {
		neg[0] = '-';
		strcpy(neg + 1, token);
	}
	
	/* Does the list contain the negative? */
	if(list_contains(l, neg)) {
		free(neg);
		return 1;
	}
	
	/* Is this a negative boolean, and if so does the scope already 
	 * include a (varname value) ? */
	if(token[0] == '-') {
		if(list_find_list(l, token + 1)) {
			free(neg);
			return 1;
		}
	}
	
	/* Otherwise apply the token. */
	if(!list_contains(l, token)) {
		list_append_token(l, token);
	}
	free(neg);
	return 0;
}

int bind_token(list * l, list * a) {
	char * lname = list_get_token(a, 1);

	/* First, check that this assignment has not been blocked by some
	 * boolean binding. */
	char * neg = malloc(strlen(lname) + 2);
	neg[0] = '-';
	strcpy(neg+1, lname);
	if(list_contains(l, neg)) {
		free(neg);
		return 1;
	}
	free(neg);
	
	
	int i;

	list * k = list_find_list(l, lname);
	
	if(!k) {
		list_append_copy(list_append_list(l), a);
		return 0;
	}

	for(i = 2; i <= a->length; i++) {
		char * t = list_get_token(a, i);
		if(!list_contains(k, t)) return 1;
	}
	
	return 0;
}

int bind(list * l, list * a) {
	int i;

	char * c;
	list * t;
	for(i = 1; i <= l->length; i++) {
		if((c = list_get_token(l, i))) {
			if(bind_bool(a, c)) return 1;
		}
		else if ((t = list_get_list(l, i))) {
			if(bind_token(a, t)) return 1;
		}
	}
	return 0;
}

/* This function returns the value held in a token variable. (or the 
 * NULL pointer if it's not bound or is boolean) */
char * eval(list * namespace, char * varname) {
	list * var = list_find_list(namespace, varname);
	if(!var) return 0;
	
	return list_get_token(var, 2);
}
	
void speculate(monad * m, char * namespace, char * varname) {
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
			if(!gotcha) continue;
			
			char * value = list_get_token(gotcha, 2);
			if(list_contains(values, value)) continue;
			
			list_append_token(values, value);
			list * r = list_append_list(list_append_list(rections));
			list_append_token(r, namespace);
			list_append_copy(list_append_list(r), gotcha);
			
		}
	}
	
	/* Now let's spawn all the monads to bind them. */
	monad_spawn(m, rections, 0);
	list_free(rections);
	list_free(values);
}

char * evaluate(monad * m, list * e) {
	/* The namespace */
	char * ns = list_get_token(e,1);
	list * namespace = 0;
	
	if(!m->namespace) m->namespace = list_new();
	namespace = list_find_list(m->namespace, ns); 
	if(!namespace) {
		namespace = list_append_list(m->namespace);
		list_append_token(namespace, ns);
	}
	
	/* Then enter the right scope */
	int i;
	if(!m->scopestack) m->scopestack = list_new();
	for(i = 1; i <= m->scopestack->length; i++) {
		list * new;
		char * scopename = list_get_token(m->scopestack, i);
		if((new = list_find_list(namespace, scopename))) {
			namespace = new;
		} else {
			namespace = list_append_list(namespace);
			list_append_token(namespace, scopename);
		}
	}
	
	/* The variable's name (which can itself be evaluated) */
	char * varname = list_get_token(e,2); 
	if(!varname) return 0;
	
	/* See if this variable can just be looked up in the namespace. */
	char * retval = 0;
	retval = eval(namespace, varname);
	if(retval) return retval;
	
	speculate(m, ns, varname);
	return 0;
}

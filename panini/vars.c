#include "../monad/monad.h"
#include "tranny.h"
#include "panini.h"
#include <string.h>
#include <stdlib.h>

/* Returns 1 if the variable is bound, returns 0 otherwise */
int v_existp(list * namespace, char * key) {
	char neg[32];
	neg[0] = '-';
	strcpy(neg+1, key);
	
	if(list_find_list(namespace, key)) return 1;
	if(list_find_list(namespace, neg)) return 1;
	
	return 0;
}

/* Returns 1 if the variable is bound negative,
 * otherwise returns 0 */
int v_negativep(list * namespace, char * key) {
	char neg[32];
	neg[0] = '-';
	strcpy(neg+1, key);
	
	if(list_find_list(namespace, neg)) return 1;
	
	return 0;
}

/* Looks a variables value up, and returns it. */
char * v_lookup(list * namespace, char * key) {
	list * var = list_find_list(namespace, key);
	if(!var) return 0;
	return list_get_token(var, 2);
}


int var_vlookup(monad * m) {
	char * nsname = list_get_token(m->command, 2);
	list * ns = monad_get_namespace(m, nsname);

	char * key = list_get_token(m->command, 3);
	
	char * val = v_lookup(ns, key);
	if(!val) {
		speculate(m, nsname, key);
		return 0;
	} 
	m->result = val;
	return 0;
}

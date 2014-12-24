#include "../list/list.h"
#include <string.h>

/* This function adds the sandhi rules to the rule. */
int add_sandhi_fin(list * rule, list * osandhi) {
	list * sandhi = 0;
	int i;
	for(i = rule->length; i; i--) {
		list * r = list_get_list(rule, i);
		char * iname = list_get_token(r, 1);
		if(!strcmp(iname, "sandhi")) {
			sandhi = r;
			break;
		}
	}
	list_append_copy(sandhi, osandhi);
	return 0;
}

/* This function is like strcmp except it only looks as far as (char*)y 
 * goes. */
int end_strcmp(char * x, char * y) {
	if(strlen(x) < strlen(y)) return 0;
	return strcmp(x + strlen(x) - strlen(y), y); 

}

/* This function tests the first word in the rule and checks if it
 * matches any of the initials in the list. */
int fin_test(list * rule, list * initial) {
	/* First, find the word that we need to check. (unless there is a
	 * constituent first) */
	int i;
	list * lit = 0;
	for(i = rule->length; i; i--) {
		list * test = list_get_list(rule, i);
		char * iname = list_get_token(test, 1);
		if(!strcmp(iname, "lit")) {
			lit = test;
			break;
		}
	}
	
	if(!lit) return 0;
	char * morph = list_get_token(lit, 2);
	
	/* Then check if it starts with the right initial */
	char * test;

	for(i = 1; i <= initial->length; i++) {
		test = list_get_token(initial, i);
		if(!end_strcmp(morph, test)) {
			return 1;
		}
	}
	return 0;
}

int sandhi_final(list * command, list * input, list * output) {
	list * initial = list_get_list(command, 2);
	list * sandhi = list_get_list(command, 3);
	
	int i;
	int j;
	
	/* This piece of code crawls through all of the rules and calls the 
	 * functions up there on each one. */
	for(i = 1; i <= output->length; i++) {
		list * rules = list_get_list(output, i);
		if(!rules) continue;
		
		for(j = 1; j<=rules->length; j++) {
			list * rule = list_get_list(rules, j);
			if(!rule) continue;
			
			if(fin_test(rule, initial))
				add_sandhi_fin(rule, sandhi);
		}
	}
	return 0;
}

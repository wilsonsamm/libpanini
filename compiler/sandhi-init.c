#include "../list/list.h"
#include <string.h>

/* This function adds the sandhi rules to the rule. */
int add_sandhi(list * rule, list * osandhi) {
	list * sandhi = list_find_list(rule, "sandhi");
	list_append_copy(sandhi, osandhi);
	return 0;
}

/* This function is like strcmp except it only looks as far as (char*)y 
 * goes. */
int partial_strcmp(char * x, char * y) {
	int i;

	for(i = 0; y[i]; i++) {
		if(!x[i]) return y[i];
		if(x[i] != y[i]) return x[i] - y[i];
	}
	return 0;
}

/* This function tests the first word in the rule and checks if it
 * matches any of the initials in the list. */
int init_test(list * rule, list * initial) {
	/* First, find the word that we need to check. (unless there is a
	 * constituent first) */
	int i;
	list * lit = 0;
	for(i = 1; i <= rule->length; i++) {
		lit = list_get_list(rule, i);
		char * iname = list_get_token(lit, 1);
		if(!strcmp(iname, "into")) iname = list_get_token(lit, 3);
		if(!strcmp(iname, "constituent")) return 0;
		if(!strcmp(iname, "lit")) break;
	}
	
	lit = list_find_list(rule, "lit");
	if(!lit) return 0;
	char * morph = list_get_token(lit, 2);
	
	/* Then check if it starts with the right initial */
	char * test;

	for(i = 1; i <= initial->length; i++) {
		test = list_get_token(initial, i);
		if(!partial_strcmp(morph, test)) {
			return 1;
		}
	}
	return 0;
}

int sandhi_initial(list * command, list * input, list * output) {
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
			
//			if(list_contains(rule, "sandhi"))
				if(init_test(rule, initial))
					add_sandhi(rule, sandhi);
		}
	}
	return 0;
}

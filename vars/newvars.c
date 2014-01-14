#include "monad.h"
#include "list.h"
#include <string.h>
#include <stdlib.h>


int ns_namespace(list * ns, char * nsname) {
	return !!list_find_list(ns, nsname);
}

list * ns_get_namespace(list * ns, char * nsname) {
	list * newns = list_find_list(ns, nsname);
	if(newns) return newns; 
	
	newns = list_append_list(ns);
	list_append_token(newns, nsname);
	return newns;
}

char * negative(char * varname) {
	char * neg = malloc(strlen(varname) + 2);
	
	if(varname[0] == '-') {
		strcpy(neg, varname + 1);
	} else {
		neg[0] = '-';
		strcpy(neg+1, varname);
	}
	return neg;
}

list * monad_get_namespace(monad * m, char * nsname) {
	list * namespace;
	int i; 
	if(!m->namespace)  m->namespace  = list_new();
	if(!m->scopestack) m->scopestack = list_new();
	
	if(!(namespace = list_find_list(m->namespace, nsname))) {
		namespace = list_append_list(m->namespace);
		list_append_token(namespace, nsname);
	}
	
	if(!strcmp(nsname, "sandhi")) return namespace;
	
	for(i = 1; i <= m->scopestack->length; i++) {
		char * scopename = list_get_token(m->scopestack, i);
		char * neg = negative(scopename);
		if(list_contains(namespace, neg)) {
			m->program = DIE;
			free(neg);
			if(m->debug) {
				printf("Could not create a new scope since the scope");
				printf("name has already been negatively bound.\n");
			}
			return 0;
		}
		free(neg);
		if(list_find_list(namespace, list_get_token(m->scopestack, i))) {
			namespace = list_find_list(namespace, list_get_token(m->scopestack, i));
		} else {
			if(!(m->howtobind & CREATE)) return 0;
			namespace = list_append_list(namespace);
			list_append_token(namespace, list_get_token(m->scopestack, i));
		}
	}
	return namespace;
}

#include "../list/list.h"
#include <stdlib.h>
#include <string.h>

char * concatenate(char * a, char * b) {
	char * c = malloc(strlen(a) + strlen(b) + 1);
	strcpy(c, a);
	strcat(c, b);
	return c;
}

void substitute(char * varname, char * value, list * in, list * out) {
	int i;
	list * l;
	char * t;
	
	char * neg = concatenate("-", varname);
	char * negval = concatenate("-", value);
	
	for(i = 1; i<=in->length; i++) {
		
		if((l = list_get_list(in, i))) {
			substitute(varname, value, l, list_append_list(out));
			continue;
		}
		
		if(!(t = list_get_token(in, i))) continue;
		if(!strcmp(varname, t)) list_append_token(out, value);
		else if(!strcmp(neg, t)) list_append_token(out, negval);
		else list_append_token(out, t);
	}
	
	free(neg);
	free(negval);
}



int for_(list * command, list * input, list * output) {
	
	char * varname = concatenate(".", list_get_token(command, 2));
	list * vals = list_get_list(command, 3);
	
	int i, j;
	for(i = 3; i <= command->length; i++) {
		for(j = 1; j<=vals->length; j++) {
			
			list * com = list_get_list(command, i);
			list * newcom = list_append_list(input);
			char * value = list_get_token(vals, j);
			
			substitute(varname, value, com, newcom);
			
		}
	}

	free(varname);
	return 0;
}

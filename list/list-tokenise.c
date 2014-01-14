#include "list.h"
#include <string.h>
#include <stdio.h>

list * __list_tokeniser(list * l, const char * toks) {
//	char * delim = 0;
//	int len = 0;
	char t[1024];
	int i = 0;
	int j = 0;

	while(toks[i]) {

		// Skip spaces
		if(toks[i] == ' ') {
			i++;
			continue;
		}

		// Skip newlines
		if(toks[i] == '\n') {
			i++;
			continue;
		}

		// Skip tabs
		if(toks[i] == '\t') {
			i++;
			continue;
		}

		// Skip single line comments
		if(toks[i] == ';') {
			while(toks[i] != '\n') i++;
			continue;
		}

		// Parentheses
		if(toks[i] == '(') {
			list_append_token(l, "(");
			i++;
			continue;
		}

		if(toks[i] == ')') {
			list_append_token(l, ")");
			i++;
			continue;
		}

		// Words
		for(;;) {
			if(toks[i] == ' ') break;
			if(toks[i] == '\n') break;
			if(toks[i] == '\t') break;
			if(toks[i] == ')') break;
			if(toks[i] == '(') break;
			if(toks[i] == ';') break;

			t[j] = toks[i];
			j++;
			i++;
		}
		t[j] = '\0';
		j = 0;

		list_append_token(l, t);
	}
	return l;
}

unsigned int __list_treebuilder(list * toks, list * target, unsigned int offset) {
	char * token;
	while(offset <= toks->length) {
//		printf("%d; %d; %d\n", toks->length, toks->allocated, offset);
		token = list_get_token(toks, offset++);
		if(!token) break;

		if(!strcmp(token, "(")) {
			list * k = list_append_list(target);
			offset = __list_treebuilder(toks, k, offset);
			continue;
		}
	
		if(!strcmp(token, ")")) {
			return offset;
		}
		
		list_append_token(target, token);
		
	}

	return offset;
}
	

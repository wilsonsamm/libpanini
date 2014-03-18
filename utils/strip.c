#include <string.h>
#include "utils.h"

/* This function strips whitespace off the end of the string you pass it */
void strip(char * c) {
	for(;;) {
		int len = strlen(c) - 1;
		
		if(c[len] == ' ') {
			c[len] = '\0';
			continue;
		}
		if(c[len] == '\n') {
			c[len] = '\0';
			continue;
		}
		if(c[len] == '\t') {
			c[len] = '\0';
			continue;
		}
		break;
	}
}

int strp(monad * m, void * nothing) {
	if(m->intext)  strip(m->intext);
	if(m->outtext) strip(m->outtext);
	return 0;
}

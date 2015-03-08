#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>
#include <stdlib.h>

/* This function is like strcmp except it only looks as far as (char*)y goes */
int partialstrcmp(char * x, char * y) {
	int i;
	
	for(i = 0; y[i]; i++) {
		if(!x[i]) return y[i];
		if(x[i] != y[i]) return x[i] - y[i];
	}
	return 0;
}

/* This function sees if a string matches the intext plus the offset. 
 * If it DOES match, then:
 *   if adv is non-zero, the offset is increased appropriately, so that the next string will be matched to the next substring.
 * 
 * If it DOES NOT match, then:
 *   if kill is non-zero, the monad is killed.
 */
int scan_intext(monad * m, char * segment) {
	
	/* Check we're not about to read past the end of the string */
	if(strlen(m->intext) < m->index) {
		if(m->debug) {
			printf("Kicked the bucket because the index is beyond the end of the intext.\n");
			printf("Index: %d, Intext length: %d\n", m->index, strlen(m->intext)); 
		}
		m->alive = 0;
		return 0;
	}
	
	/* Remove the sandhi bindings. (These will not apply on the other side of the segment) */
	if(m->namespace) list_remove(m->namespace, "sandhi");
	
	/* See if the next segment matches the next section of the string */
	if(partialstrcmp(m->intext + m->index, segment)) {
		if(m->debug) {
			printf("%s does not match %s.\n", segment, m->intext + m->index);
		}
		m->alive = 0;
		return 0;
	}
	
	m->index += strlen(segment);
	
	return 1;
}

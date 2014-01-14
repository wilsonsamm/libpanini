#include "list/list.h"
#include "monad/monad.h"
#include <stdio.h>
#include <stdlib.h>

/* This program can be used to test the various functions in the source 
 * tree. Run in through valgrind or lint or something, and use it to 
 * verify proper behaviour! */

int main(int argv, char * argc[]) {

	list * t = list_new();
	list_tokenise_chars(t, "  (this is a test (1 2 3   )) (another one) ");
	
	list_tokenise_chars(t, "((((((((this is another test(4 5 6)))))))))");

	list * l = list_append_list(t);
	list_append_token(l, "hello,");
	list_append_token(l, "world!");

	list * n = list_append_list(l);
	list_append_token(n, "1");
	list_append_token(n, "2");
	list_append_token(n, "3");
	
	list_prettyprinter(t);
	list_free(t);

	FILE * fp = fopen("/usr/tranny/languages/nahuatl", "r");
	list * f = list_new();
	list_tokenise_file(f, fp);
	fclose(fp);

	printf("\n");
	list_prettyprinter(f);

	list_free(f);

	return 0;
}

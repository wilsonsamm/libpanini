
#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * This program uses libtranny to generate a word list for a particular language. To use this program, type:
 *  $ dict [language]
 */
 
int main(int argc, char * argv[]) {
	
	/* Create a new monad and load the rules for the specified language. */
	monad * m = monad_new();
	monad_rules(m, argv[1]);
	
	int t = 6;
	monad_map(m, set_trace, &t, 0);
	
	/* Generate words */
	monad_map(m, set_stack, "(constituent Word)", 0);
	monad_map(m, tranny_gowild, (void *)0, 0);
	
	/* Only keep unique words */
	monad_map(m, kill_identical_outtexts, (void *)0, 0);
	
	/* And print them out. */
	monad_map(m, print_out, stdout, 0);
	
	/* Free the monad. */
	monad_free(m);
	return 0;
}

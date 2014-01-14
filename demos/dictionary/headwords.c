
#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// To use this program, type:
//  $ dict srclang dstlang

int main(int argc, char * argv[]) {
	/* Open the headwords file. This is what we'll use to store the headwords */
	FILE * headwords = fopen("headwords", "w");
	
	/* create a new monad. */
	monad * m = monad_new();
	
	/* load the appropriate languages */
	monad_rules(m, argv[1]);
	
	/* Generate headwords */
	monad_map(m, set_stack, argv[2], 0);
	monad_map(m, tranny_gowild, (void *)0, 0);
	monad_map(m, kill_identical_outtexts, (void *)0, 0);
	
	/* Print all the headwords out to that file. */
	monad_map(m, print_out, headwords, 0);
	fclose(headwords);
	
	/* free the monads. */
	monad_free(m);
	return 0;
}

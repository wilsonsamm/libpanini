
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panini.h>	

int main(int argc, char * argv[]) {
	
	/* create a new monad. */
	monad * m = monad_new();
	
	/* Set the rules ready for translation from the original */
	monad_rules(m, argv[1]);

	/* Make the translations. */
	panini_generate(m, argv[2], 0, 5);
	panini_generate(m, argv[2], 0, 5);
	
	/* And spit the output out! */
	FILE * output = fopen(argv[3], "a+");
	monad_map(m, print_out, output, 5);
	fclose(output);
	monad_free(m);
	return 0;
	
}

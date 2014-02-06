#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// To use this program, type:
//  $ tranny srclang dstlang [OPTIONS ...] -- Sentence goes here.

int cl_verbose = 0;
int cl_best = 0;
int cl_one = 0;
int cl_none = 0;
int cl_trim = 0;

int main(int argc, char * argv[]) {

	int threshold = 20;
	int retval;
	
	/* create a new monad. */
	monad * m = monad_new();

	/* Check for command line sanity. */
	if(argc < 5) {
		printf("To use this program, type:\n");
		printf(" $ tranny srclang dstlang [OPTIONS ...] -- Sentence goes here.\n");
		return 1;
	}

	/* read all the command line options. */
	int i = 3;
	while(strcmp(argv[i],"--")) {	/* find the "--" on the command line */
		if(!strcmp(argv[i], "none")) {	// only give one translation
			cl_none = 1;
		} else
		if(!strcmp(argv[i], "one")) {	// only use one of the interpretations
			cl_one = 1;
		} else
		if(!strcmp(argv[i], "best")) {	// produce the best translation
			cl_best = 1;
		} else
		if(!strcmp(argv[i], "verbose")) {	// Verbose
			cl_verbose = 1;
		} else
		if(!strcmp(argv[i], "trim")) {	// Verbose
			cl_trim = 1;
		} else
		if(!strcmp(argv[i], "d")) {		// Debug a monad (takes numerical argument)
			int t = atoi(argv[++i]);
			monad_map(m, set_trace, &t, -1);
		} else
		if(!strcmp(argv[i], "t")) {		// Set threshold to something other than the default 20 (takes numerical argument)
			int t = atoi(argv[++i]);
			monad_map(m, set_trace, &t, -1);
		}else {
			printf("No such option as: %s\n",argv[i]);
			exit(99);
		}
		i++;
	}
	i++;
	
	/* Read the sentence and set the INTEXT register in the monad. */
	char * sentence = strdup("");
	while(argv[i]) {
		char * tmp = malloc(strlen(sentence) + strlen(argv[i]) + 2);
		strcpy(tmp, sentence);
		strcpy(tmp + strlen(tmp), argv[i]);
		strcpy(tmp + strlen(tmp), " ");
		free(sentence);
		sentence = tmp;
		i++;
	}
	monad_map(m, set_intext, (void *)sentence, -1);
	
	/* Set the rules ready for translation from the original */
	monad_rules(m, argv[1]);
	
	/* Parse! */
	monad_map(m, set_stack, "(constituent main)", -1);
	for(i = 0; i < threshold; i++) {
		retval = monad_map(m, tranny_parse, argv[1], i);
		if(retval) break;
	}
	
	/* If the parse wasn't successful, then exit. */
	if(!retval) {
		if(cl_verbose) {
			printf("Could not parse the input; maybe you put gibberish in.\n");
			printf("You can also try a higher threshold with the commandline option \"t [number]\".\n");
		}
		monad_free(m);
		exit(1);
	}
	monad_map(m, kill_not_done, (void*)0, -1);
	i++;
	
	/* Do we want to only use the most likely interpretation? */
	if(cl_one) monad_map(m, kill_least_confident, (void *)0, -1);
	
	/* Did the user want to see all the namespaces? */
	if(cl_verbose) monad_map(m, print_ns, stdout, -1);
	
	/* Set the rules ready for translation to the wanted language (but of course we don't need to do that if the source language 
	 * and destination language are the same, or if "none" is specified). */
	if(strcmp(argv[2], "none")) {
		if(strcmp(argv[1], argv[2]))
			monad_rules(m, argv[2]);
	} else {
		cl_none = 1;
	}
	
	/* Generate! */
	if(!cl_none) {
		monad_map(m, remove_ns, "rection", -1);
		monad_map(m, remove_ns, "theta", -1);
		monad_map(m, set_stack, "(constituent main)", -1);
		
		for(; i <= threshold; i++) {
			retval = monad_map(m, tranny_generate, (void *)0, i);
			if(retval) break;
		}
		if((!retval) && cl_verbose) {
			printf("I understood the input but I don't know of a way to say it in %s.\n", argv[2]);
			printf("Try another language maybe?\n");
		}
		
		monad_map(m, kill_identical_outtexts, (void *)0, -1);
		
		monad_map(m, print_out, stdout, i);
	}
	
	
	monad_free(m);
	free(sentence);
	return 0;
	
}

#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// To use this program, type:
//  $ tranny srclang dstlang [OPTIONS ...] -- Sentence goes here.

char * cl_kyouiku = "6";

int main(int argc, char * argv[]) {

	int retval;
	
	/* create a new monad. */
	monad * m = monad_new();

	/* Check for command line sanity. */
	if(argc < 5) {
		printf("To use this program, type:\n");
		printf(" $ kanjify [OPTIONS ...] -- Sentence goes here.\n");
		printf("one possible option is -kyouiku, which specifies the level of kanji you want. An example:\n");
		printf(" $ kanjify -kyouiku 4 watashi ga aisuru\n");
		printf("should return something like 私があいする, because 私 is a Kyouiku Level 4 Kanji (or less), but ");
		printf("愛 isn't (it's level 6).\n");
		return 1;
	}

	/* read all the command line options. */
	int i = 3;
	while(strcmp(argv[i],"--")) {	/* find the "--" on the command line */
		if(!strcmp(argv[i], "-kyouiku")) {	// only give one translation
			cl_kyouiku = argv[++i];
		} 
		if(!strcmp(argv[i], "d")) {		// Debug a monad (takes numerical argument)
			int t = atoi(argv[++i]);
			monad_map(m, set_trace, &t, -1);
		} else {
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
	monad_rules(m, "japanese");
	
	/* Parse! */
	monad_map(m, set_stack, "(constituent main)", 20);
	retval = monad_map(m, tranny_parse, (void *)0, 20);

	/* If the parse wasn't successful, then exit. */
	if(!retval) {
		monad_free(m);
		exit(1);
	}
	monad_map(m, kill_not_done, (void*)0, -1);
	i++;
	
	/* Generate! */

	/* Construct the code to execute */
	char * exec;
	exec = malloc(strlen("(constituent main) (language (kyouiku )) ") + strlen(cl_kyouiku) + 1);
	strcpy(exec, "(language (kyouiku ");
	strcat(exec, cl_kyouiku);
	strcat(exec, ")) ");
	strcat(exec, "(constituent main)");

	/* Prepare the monads for generation */
	monad_map(m, remove_ns, "rection", -1);
	monad_map(m, remove_ns, "theta", -1);
	monad_map(m, remove_ns, "language", -1);
	
	monad_map(m, set_stack, exec, -1);
	
	retval = monad_map(m, tranny_generate, (void *)0, 20);
	
	/* Only keep the best interpretation if that's what the user requested */
	monad_map(m, kill_least_confident, (void*)0, -1);
	monad_map(m, kill_identical_outtexts, (void *)0, -1);
	monad_map(m, print_out, stdout, i);
	
	monad_free(m);
	free(sentence);
	if(retval) return 0;
	exit(2);
}

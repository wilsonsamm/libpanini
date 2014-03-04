#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
	
	/* create a new monad. */
	monad * m = monad_new();

	/* Check for command line sanity. */
	if(argc < 3) {
		printf("To use this program, type:\n");
		printf(" $ tranny language Sentence goes here.\n");
		return 1;
	}
	/* read all the command line options. */
	int i = 2;
	while(strcmp(argv[i],"--")) {	/* find the "--" on the command line */
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
	
	/* read all the command line options. */
	i = 2;
	while(strcmp(argv[i],"--")) {	/* find the "--" on the command line */
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
	
	/* Load the language rules */
	monad_rules(m, argv[1]);
	
	/* Parse! */
	monad_map(m, set_stack, "(constituent main)", -1);
	monad_map(m, tranny_attest, argv[1], 20);
	
	monad_map(m, kill_not_done, (void*)0, -1);
	i++;
	
	monad_map(m, print_out, stdout, i);
	
	monad_free(m);
	free(sentence);
	return 0;
}

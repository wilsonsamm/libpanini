#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// To use this program, type:
//  $ tranny srclang dstlang [OPTIONS ...] -- Sentence goes here.

char * cl_kyouiku = "6";

void kyouiku(monad * m) {
	char exec[1024];
	
	strcpy(exec, "(language (kyouiku ");
	strcat(exec, cl_kyouiku);
	strcat(exec, "))");
	monad_map(m, set_stack, exec, -1);
	
	monad_map(m, tranny_generate, (void*)0, -1);
}

int main(int argc, char * argv[]) {

	int retval;
	
	/* create a new monad. */
	monad * m = monad_new();

	/* Check for command line sanity. */
	if(argc == 1) {
		printf("To use this program, type:\n");
		printf(" $ kanjify [OPTIONS ...] -- Sentence goes here.\n");
		printf("one possible option is -kyouiku, which specifies the level of kanji you want. An example:\n");
		printf(" $ kanjify -kyouiku 4 -- watashi ga aisuru\n");
		printf("should return something like わたしがあいする, because 愛 is a Kyouiku Level 4 Kanji (or less), but ");
		printf("私 isn't (it's level 6).\n");
		return 1;
	}

	/* read all the command line options. */
	int i = 1;
	while(argv[i][0] == '-') {	/* find everything beginning with a dash */
		if(!strcmp(argv[i], "-kyouiku")) {	// select the kyouiku level
			cl_kyouiku = argv[++i];
			//printf("cl_kyouiku now reads %s\n", cl_kyouiku);
			//i++;
		} else
		if(!strcmp(argv[i], "-d")) {		// Debug a monad (takes numerical argument)
			int t = atoi(argv[++i]);
			monad_map(m, set_trace, &t, -1);
		} else {
			printf("No such option as: %s\n",argv[i]);
			exit(99);
		}
		i++;
	}
	
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
	monad_map(m, set_stack, "(language -kanji) (constituent main)", 20);
	retval = monad_map(m, tranny_parse, (void *)0, 20);

	/* If the parse wasn't successful, then exit. */
	if(!retval) {
		monad_free(m);
		exit(1);
	}
	monad_map(m, kill_not_done, (void*)0, -1);
	i++;
	
	/* Prepare the monads for generation */
	monad_map(m, remove_ns, "rection", -1);
	monad_map(m, remove_ns, "theta", -1);
	monad_map(m, remove_ns, "language", -1);

	/* Select the kyouiku level */
	if(cl_kyouiku) kyouiku(m);
	
	monad_map(m, set_stack, "(call main)", -1);
	
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

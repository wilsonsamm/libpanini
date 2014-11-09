#include <panini.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// To use this program, type:
//  $ tranny srclang dstlang [OPTIONS ...] -- Sentence goes here.

char * cl_kyouiku = "6";
int cl_nokanji = 0;

void kyouiku(monad * m) {
	char exec[1024];
	
	strcpy(exec, "(language (kyouiku ");
	strcat(exec, cl_kyouiku);
	strcat(exec, "))");
	
	panini_parse(m, exec, "", 0, 0, -1);
}

void nokanji(monad * m) {
	panini_parse(m, "(language -kanji)", "", 0, 0, -1);
}

int main(int argc, char * argv[]) {

	int retval;
	
	/* create a new monad. */
	monad * m = monad_new();

	/* Check for command line sanity. */
	if(argc == 1) {
		printf("To use this program, type:\n");
		printf(" $ kanjify [OPTIONS ...] -- Sentence goes here.\n\n");
		printf("Possible options include:\n");
		printf(" -kyouiku    This takes a numerical argument and specifies what level of Kanji \n");
		printf("             you want putting in. The argument, 1-6, specifies the year of \n");
		printf("             primary school when the kanji is learnt.\n");
		printf(" -nokanji    If you put this in, then no kanji will be used to transliterate\n");
		printf("             your input; only kana.\n");
		
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
		if(!strcmp(argv[i], "-nokanji")) {	// select the kyouiku level
			cl_nokanji = 1;
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

	/* Set the rules ready for translation from the original */
	monad_rules(m, "japanese");
	
	/* Parse! */
	retval = panini_parse(m, "(language -kanji) (call main)", sentence, 0, 1, 20);
	/* If the parse wasn't successful, then exit. */
	if(!retval) {
		fprintf(stderr, "I couldn't work your sentence out.\n");
		monad_free(m);
		exit(1);
	}
	
	monad_map(m, kill_least_confident, (void *)0, -1);
	i++;
	
	/* Prepare the monads for generation */
	monad_map(m, remove_ns, "rection", -1);
	monad_map(m, remove_ns, "theta", -1);
	monad_map(m, remove_ns, "language", -1);

	/* Select the kyouiku level */
	if(cl_nokanji) nokanji(m);
	if(cl_kyouiku) kyouiku(m);
	
	retval = panini_generate(m, "(call main)", 1, -1);
	
	/* Only keep the best result if that's what the user requested */
	monad_map(m, kill_least_confident, (void*)0, -1);
	monad_map(m, kill_identical_outtexts, (void *)0, -1);
	monad_map(m, print_out, stdout, i);
	
	monad_free(m);
	free(sentence);
	
	if(retval) return 0;
	exit(2);
}

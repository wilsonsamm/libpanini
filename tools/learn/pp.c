#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../panini.h"
#include "readfile.h"

#define EXEC1 "(language (stage "
#define EXEC2 ")) (call parameters) (call main)"
char * stage = 0;
char * lang1;
char * lang2;

char * path_to_language(char * lang) {

	if(!getenv("PANINI")) {
		fprintf(stderr, "No variable PANINI.\n");
		exit(99);
	}

	char * fn = malloc(strlen(getenv("PANINI")) + strlen("/languages/") + \
	                  (strlen(lang) *2) + 2);
	strcpy(fn, getenv("PANINI"));
	strcat(fn, "/languages/");
	strcat(fn, lang);
	strcat(fn, "/");
	strcat(fn, lang);
	return fn;
}

int parsesection(FILE * fp) {

	FILE * outfile = fopen("pp.out", "a");
	
	/* get the language name and sentence from the first line in the 
	 * section. 
	 * A blank line ends the section and the next line after that, if it 
	 * contains any text, is the first line of the next section. */
	nextline(fp);
	if(!currentline) {
		if(lang1) free(lang1);
		if(lang2) free(lang2);
		lang1 = 0;
		lang2 = 0;
		return 0;
	}
	if(!strlen(currentline)) {
		if(lang1) free(lang1);
		if(lang2) free(lang2);
		lang1 = 0;
		lang2 = 0;
		return 1;
	}
	
	char * srclang = getfield(1);
	char * srctext = getfield(2);
	char * srcfn = path_to_language(srclang);
	
	monad * m = monad_new();
	monad_rules(m, srcfn);
		
	/* Let's keep the user up-to-date with what's happening. */
	progpc(NONE, 0);
	
	int retval = panini_parse(m, "(call main)", srctext, 0, 0, 5);
	
	if(!retval) {
		progpc(SAD, srctext);
	}
	
	/* Prepare the monads for generation */
	monad_map(m, remove_ns, "rection", -1);
	monad_map(m, remove_ns, "record", -1);
	monad_map(m, remove_ns, "theta", -1);
	
	while(strlen(currentline)) {
		nextline(fp);
		
		char * lang = getfield(1);
		if(!lang) return 1;
		if(!strlen(lang)) return 1;
		
		monad * test = monad_duplicate(m);
		if(!test) continue;
		
		monad * n = monad_duplicate(m);
		if(!n) continue;
		
		char * fn = path_to_language(lang);
		free(lang);
		
		char * txt = getfield(2);
		monad_rules(n, fn);
		monad_rules(test, fn);
		
		/* Is this sentence already known in this language? 
		 * If so, don't bother trying to learn anything. */
		if(panini_parse(m, "(call main)", txt, 0, 0, 20)) {
			monad_free(test);
			monad_free(n);
			continue;
		}
		
		progpc(HAPPY, 0);
		
		char * exec = malloc(strlen(EXEC1) + strlen(EXEC2) + 20);
		strcpy(exec, EXEC1);
		strcat(exec, stage);
		strcat(exec, EXEC2);
		
		if(!panini_learn(n, exec, outfile, txt, 20)) {
			progpc(SAD, txt);
		} else {
			progpc(DOTS, 0);
		}
		free(exec);
	}
	return 1;
}

int main(int argc, char * argv[]) {

	/* Open the textfile that has all the sentences in it. */
	FILE * fp = fopen("data.txt", "r");
	if(!fp) {
		fprintf(stderr,"Cannot open %s.\n", argv[1]);
		exit(1);
	}
	
	stage = argv[2];
	
	count_lines(fp);
	
	while(parsesection(fp));
	
	progpc(DONE, 0);
	fclose(fp);
	return 0;
}

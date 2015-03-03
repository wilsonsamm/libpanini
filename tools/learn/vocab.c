
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../panini.h"
#include "readfile.h"

char * lang1;
char * lang2;

char * path_to_language(char * lang) {
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

	FILE * outfile = fopen("vocab.out", "a");
	
	/* get the language name and sentence from the first line in the 
	 * section. */
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
	
	lang1 = getfield(1);
	lang2 = getfield(2);
	char * fn1 = path_to_language(lang1);
	char * fn2 = path_to_language(lang2);
	
	/* what to execute for each language */
	nextline(fp);
	char * exec1 = getfield(1);
	char * exec2 = getfield(2);
		
	/* Let's keep the user up-to-date with what's happening. */
	progpc(NONE, 0);
		
	/* Find out what the sentence means */
	if(!getenv("PANINI")) {
		fprintf(stderr, "No variable PANINI.\n");
		exit(99);
	}
	
	nextline(fp);
	while(strlen(currentline)) {
		
		char * text1 = getfield(1);
		char * text2 = getfield(2);

		clear_progpc();
		printf(" vocab: %s\r", text2);
		progpc(NONE, 0);
		
		monad * m = monad_new();
		monad_rules(m, fn1);
		
		if(!panini_parse(m, exec1, text1, 0, 0, 5)) {
			progpc(SAD, 0);
		} else {
			
			/* Prepare the monads for generation */
			monad_map(m, remove_ns, "rection", -1);
			monad_map(m, remove_ns, "record", -1);
			monad_map(m, remove_ns, "theta", -1);
			
			progpc(HAPPY, 0);
			monad_rules(m, fn2);
			
			if(!panini_learnvocab(m, exec2, outfile, text2, 20)) {
				progpc(SAD, 0);
			} else {
				progpc(DOTS, 0);
			}
		} 

		free(text1);
		free(text2);
		nextline(fp);
		monad_free(m);
		
		if(!currentline || (currentline && strlen(currentline) == 1)) {
			break;
		}
		
	}
	fclose(outfile);
	free(fn1);
	free(fn2);
	free(exec1);
	free(exec2);
	return 1;
}

int main(int argc, char * argv[]) {

	/* Open the textfile that has all the sentences in it. */
	FILE * fp = fopen(argv[1], "r");
	if(!fp) {
		fprintf(stderr,"Cannot open %s.\n", argv[1]);
		exit(1);
	}
	
	count_lines(fp);
	
	while(parsesection(fp));
	
	progpc(DONE, 0);
	fclose(fp);
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../panini.h"

#define INFILE "data.txt"

#define EXEC1 "(language (stage "
#define EXEC2 ")) (call parameters) (call main)"

#define NONE  0
#define HAPPY 1
#define SAD   2
#define DONE  3
#define DOTS  4

int totallines;
int progress = 0;

char * stage;
char * learnlang = 0;

int count_lines(FILE * fp) {
	rewind(fp);
	int c = fgetc(fp);
	int i = 1;
	while(c != EOF) {
		if(c == '\n') i++;
		c = fgetc(fp);
	}
	rewind(fp);
	return i;
}

void progpc(int smile, char * text) {
	printf("Stage %s of learning %s with pp", stage, learnlang);
	printf(" %d%%", (progress * 100/totallines * 100)/100);
	//printf(", line %d,", progress);
	if(smile == DONE)  printf(" All done!\n");
	if(smile == NONE)  printf("    \r");
	if(smile == HAPPY) printf(" :-)\r");
	if(smile == SAD)   printf(" :-(  line %d: %s\n", progress, text);
	if(smile == DOTS)  printf(" ...\r");
	fflush(stdout);
}

/* 
 * This returns the name of the language referred to by a line in the
 * data file, which is the string before the first colon. 
 */
char * getlang(char * line) {
	char lang[32];
	
	strncpy(lang, line, 32);
	
	char * colon = strstr(lang, ":");
	colon[0] = '\0';
	
	return strdup(lang);
}

/* 
 * This returns the text on the line, which is anything after the first
 * colon and space.
 */
char * gettext(char * line) {
	return strdup(strstr(line, ": ") + strlen(": "));
}

int parsesection(FILE * fp) {

	monad * m = monad_new();
	
	char * line = NULL;
	size_t len = 0;
	
	/* get the language name and sentence from the first line in the 
	 * section. */
	int retval = getline(&line, &len, fp);
	if(!line) return 0;
	
	if(retval == -1) {
		if(line) free(line);
		return 0;
	}
	progress++;
	char * srclang = getlang(line);
	if(!srclang) {
		free(line);
		return 1;
	}
	char * srctext = gettext(line);
	if(!srctext) {
		free(line);
		return 1;
	}
	
	/* Let's keep the user up-to-date with what's happening. */
	progpc(NONE, 0);
		
	/* Find out what the sentence means */
	if(!getenv("PANINI")) {
		fprintf(stderr, "No variable PANINI.\n");
		exit(99);
	}
	char * fn = malloc(strlen(getenv("PANINI")) + strlen("/languages/") +(strlen(srclang) *2) + 2);
	strcpy(fn, getenv("PANINI"));
	strcat(fn, "/languages/");
	strcat(fn, srclang);
	strcat(fn, "/");
	strcat(fn, srclang);
	//fprintf(stderr, "Opening rules in %s.\n\n\n", fn);
	
	monad_rules(m, fn);
	if(!panini_parse(m, "(call main)", srctext, 0, 0, 5)) {
		monad_map(m, print_ns, stdout, -1);
		progpc(SAD, line);
	}
	/* Prepare the monads for generation */
	monad_map(m, remove_ns, "rection", -1);
	monad_map(m, remove_ns, "record", -1);
	monad_map(m, remove_ns, "theta", -1);
	
	free(srclang);
	free(srctext);
	
	/* For all remaining lines in the section, */
	retval = getline(&line, &len, fp);
	progress++;
	if(retval == -1) {
		free(line);
		monad_free(m);
		return 0;
	}
	while(strstr(line, ":")) {

		/* Let's keep the user up-to-date with what's happening. */
		progpc(NONE, 0);

		/* Get the language from the line we're currently processing */
		char * dstlang = getlang(line);
		
		/* If that language is not the same as the language we want to learn,
		 * then we need to skip it.
		 */
		if(learnlang && strcmp(learnlang, dstlang)) {
			retval = getline(&line, &len, fp);
			progress++;
			free(dstlang);
			if(retval == -1) {
				monad_free(m);
				free(line);
				return 0;
			}
			continue;
		}
		char * dsttext = gettext(line);
		
		/* Copy the monad */
		monad * n = monad_duplicate(m);
		if(!n) break;
		char * fn = malloc(strlen("./") + strlen(dstlang) + 1);
		strcpy(fn, "./");
		strcat(fn, dstlang);
		//fprintf(stderr, "Opening rules in %s.\n\n\n", fn);
		
		monad_rules(n, fn);
		free(fn);
		
		/* Create the directory if needed */
		char * filename = malloc(strlen(dstlang) + strlen("-pp-out") + 1);
		strcpy(filename, dstlang);
		strcat(filename, "-pp-out");
		
		/* Open a new file in that directory */
		FILE * out = fopen(filename, "a");
		free(filename);
		free(dstlang);
		
		if(!out) {
			printf("Could not open %s for writing.\n", filename);
			exit(99);
		}
		
		/* Try to do some learning (and turn the smiley on while we do) */
		char * exec = malloc(strlen(EXEC1) + strlen(EXEC2) + strlen(stage) + 1);
		strcpy(exec, EXEC1);
		strcat(exec, stage);
		strcat(exec, EXEC2);
		progpc(HAPPY, 0);
		int retval = panini_learn(n, exec, out, dsttext, 20);
		if(!retval) progpc(SAD, line);
		else progpc(NONE, 0);
		free(exec);
		
		/* Tidy up */
		monad_free(n);
		free(dsttext);
		fclose(out);
		
		/* Try the next line out */
		retval = getline(&line, &len, fp);
		progress++;
		progpc(NONE, 0);

		if(retval == -1) {
			free(line);
			monad_free(m);
			return 0;
		}
	}
	progpc(NONE, 0);
	monad_free(m);
	free(line);
	return 1;
}

int main(int argc, char * argv[]) {

	/* Open the textfile that has all the sentences in it. */
	FILE * fp = fopen(INFILE, "r");
	if(!fp) {
		fprintf(stderr,"Cannot open %s.\n", INFILE);
		exit(1);
	}
	
	learnlang = argv[1];
	stage = argv[2];

	totallines = count_lines(fp);
	
	while(parsesection(fp));

	progpc(DONE, 0);
	fclose(fp);
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <panini.h>

#define INFILE "data.txt"

char * learnlang = 0;

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
 * colon space.
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
	
	/* Find out what the sentence means */
	monad_rules(m, srclang);
	if(!panini_parse(m, "(call main)", srctext, 0, 0, 5)) {
		monad_map(m, print_ns, stdout, -1);
		printf("I don't understand the %s phrase \"%s\".\n", srclang, srctext);
		exit(99);
	}
	/* Prepare the monads for generation */
	monad_map(m, remove_ns, "rection", -1);
	monad_map(m, remove_ns, "record", -1);
	monad_map(m, remove_ns, "theta", -1);
	
	free(srclang);
	free(srctext);
	
	/* For all remaining lines in the section, */
	retval = getline(&line, &len, fp);
	if(retval == -1) {
		free(line);
		monad_free(m);
		return 0;
	}
	while(strstr(line, ":")) {
		
		char * dstlang = getlang(line);
		if(learnlang && strcmp(learnlang, dstlang)) {
			retval = getline(&line, &len, fp);
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
		monad_rules(n, "pp");
		
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
		
		/* Try to do some learning */
		panini_learn(n, "(call main)", out, dsttext, 20);
		
		/* Tidy up */
		monad_free(n);
		free(dsttext);
		fclose(out);
		
		/* Try the next line out */
		retval = getline(&line, &len, fp);
		if(retval == -1) {
			free(line);
			monad_free(m);
			return 0;
		}
	}
	monad_free(m);
	free(line);
	return 1;
}

int main(int argc, char * argv[]) {

	/* Open the spreadsheet that has all the data in it. */
	FILE * fp = fopen(INFILE, "r");
	if(!fp) {
		fprintf(stderr,"Cannot open %s.\n", INFILE);
		exit(1);
	}
	
	learnlang = argv[1];
	
	while(parsesection(fp));
	
	fclose(fp);
	return 0;
}

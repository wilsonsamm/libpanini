#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../panini.h"
#include "readfile.h"

char * srclang = 0;
char * dstlang = 0;
char * srcexec = 0;
char * dstexec = 0;
char * srctext = 0;
char * dsttext = 0;
char * label = 0;
char * outfile = 0;

char * stage = 0;
char * lang1;
char * lang2;

char * path_to_language(char * lang) {

	if(!getenv("PANINI")) {
		fprintf(stderr, "No variable PANINI.\n");
		exit(99);
	}

	char * fn = malloc(strlen(getenv("PANINI")) + strlen("/targets/") + \
	                  (strlen(lang) *2) + 2);
	strcpy(fn, getenv("PANINI"));
	strcat(fn, "/targets/");
	strcat(fn, lang);
	strcat(fn, "/");
	strcat(fn, lang);
	return fn;
}

int parsesection(FILE * fp) {

	/* get the language name and sentence from the first line in the 
	 * section. 
	 * A blank line ends the section and the next line after that, if it 
	 * contains any text, is the first line of the next section. */
	nextline(fp);
		
	/* Let's keep the user up-to-date with what's happening. */
	clear_progpc();
	progpc(NONE, label);
	
	if(!currentline) {
		return 0;
	}
	if(!strlen(currentline)) {
		return 1;
	}
	
	char * tag = getfield(1);
	char * value = getfield(2);

	if(!strlen(tag)) {
		return 1;
	}
	
	if(!strcmp(tag, "srclang")) {
		if(srclang) free(srclang);
		srclang = path_to_language(value);
		return 1;
	}
	if(!strcmp(tag, "dstlang")) {
		if(dstlang) free(dstlang);
		dstlang = path_to_language(value);
		return 1;
	}
	if(!strcmp(tag, "srcexec")) {
		if(srcexec) free(srcexec);
		srcexec = value;
		return 1;
	}
	if(!strcmp(tag, "dstexec")) {
		if(dstexec) free(dstexec);
		dstexec = value;
		return 1;
	}
	if(!strcmp(tag, "srctext")) {
		if(srctext) free(srctext);
		srctext = value;
		return 1;
	}
	if(!strcmp(tag, "dsttext")) {
		if(dstexec) free(dsttext);
		dsttext = value;
		return 1;
	}
	if(!strcmp(tag, "label")) {
		if(label) free(label);
		label = value;
		return 1;
	}
	if(!strcmp(tag, "outfile")) {
		if(outfile) free(outfile);
		outfile = value;
		return 1;
	}

	if(!strcmp(tag, "go") && !strcmp(value, "vocab") ) {
		monad * m = monad_new();
		monad_rules(m, srclang);
	
		/* Prepare the monads for generation */
		monad_map(m, remove_ns, "rection", -1);
		monad_map(m, remove_ns, "record", -1);
		monad_map(m, remove_ns, "theta", -1);

		monad_rules(m, dstlang);
		
		/* If the word is already known, then skip it! */
		monad * n = monad_duplicate(m);
		if(panini_parse(m, dstexec, dsttext, 0, 0, 20)) {
			monad_free(m);
			return 1;
		}

		FILE * out = fopen(outfile, "a");
		panini_learnvocab(n, dstexec, out, dsttext, 20);
		
		fclose(out);
		monad_free(m);
		monad_free(n);
		return 1;
	}

	if(!strcmp(tag, "go") && !strcmp(value, "bootstrap")) {
		monad * m = monad_new();
		monad_rules(m, dstlang);
		FILE * out = fopen(outfile, "a");

		panini_learnvocab(m, dstexec, out, dsttext, 20);
		
		fclose(out);
		monad_free(m);
		return 1;
	}
	
	fprintf(stderr, "unknown tag %s\n", value);
	return 1;
}

int main(int argc, char * argv[]) {
	FILE * fp = fopen(argv[1], "r");
	if(!fp) {
		fprintf(stderr,"Cannot open %s.\n", argv[1]);
		exit(1);
	}
	count_lines(fp);
	rewind(fp);
	while(parsesection(fp));
	
	progpc(DONE, label);
	return 0;
}

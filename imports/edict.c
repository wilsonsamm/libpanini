#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <panini.h>
#include "read.h"

/* 
 * This program reads in all the entries in the edict.locale file. This is 
 * automatically adapted (see Makefile) from the edict file prepared by Jim
 * Breen and others at the Monash University. I extend a gesture of gratitude
 * to those who made this possible.
 * 
 * (The file itself is copied from /usr/share/edict/, where Debian puts the 
 * file.)
 * 
 * This program is part of the Panini Project, which aims to unify various 
 * computer-readable dictionaries and provide something of a computational
 * linguistics Swiss Army knife. 
 * 
 * This program reads in edict and spits out Panini source code (the 
 * functional equivalent of (mostly) the same data, but usable by
 * libpanini.
 */

int learnentry(char * headword, char * reading, char * translation) {
	monad * m = monad_new();
	
	/* Read the translation in and generate the meaning. */
	monad_map(m, set_intext, (void *)translation, -1);
	monad_rules(m, "english");
	monad_map(m, set_stack, "(constituent Headword)", -1);
	if(!monad_map(m, tranny_parse, (void *)0, 20)) {
		//fprintf(stderr, "Could not learn %s: could not interpret %s.\n", headword, translation);
		monad_free(m);
		return 1;
	} else {
		fprintf("Learning %s [%s] as \"%s\".\n", headword, reading, translation);
	}
	
	/* Read the headword in and "learn" it. */
	monad_map(m, set_intext, (void *)headword, -1);
	monad_rules(m, "japanese");
	monad_map(m, set_stack, "(constituent Learn)", -1);
	if(!monad_map(m, tranny_learn, (void *)"japanese", 20)) {
		//fprintf(stderr, "Could not learn %s\n", headword);
		monad_free(m);
		return 1;
	}
	
	/* Read the reading in and "attest" it. */
	monad_map(m, set_intext, reading, -1);
	monad_map(m, set_stack, "(constituent Learn)", -1);
	if(!monad_map(m, tranny_attest, (void *)"japanese", 20)) {
		//fprintf(stderr, "Could not attest %s\n", reading);
		monad_free(m);
		return 1;
	}
	
	return 0;
}

int readentry(FILE * kanjidic, char * headword) {
	char * line = readline(kanjidic);
	
	if(!strstr(line, "[")) return 1;
	if(!strstr(line, "]")) return 1;
	
	char * reading = strdup(strstr(line, "[") + strlen("["));
	char * nulterm = strstr(reading, "]");
	nulterm[0] = '\0';
	
	char * translation = strstr(line, "/") + strlen("/");
	while(translation) {
		learnentry(headword, reading, translation);
		if(!strstr(translation, "/")) break;
		translation = strstr(translation, "/") + strlen("/");
	}
	
	return 0;
}

int main(int argc, char * argv[]) {
	
	FILE * edict = fopen("./edict.locale", "r");
	
	/* The first line is only a comment */
	skip_line(edict);
	
	/* Read the file in line by line */
	for(;;) {
		
		/* Read in the headword. */
		char * headword = readword(edict);
		if(!headword) break;
		if(!strlen(headword)) break;
		fprintf(stderr, "%s ", headword);
		readentry(edict, headword);
	}
	return 0;
}

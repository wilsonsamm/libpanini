#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "reading.h"
#include "read.h"

char * append_string(char * x, char * y) {
	char * retval = malloc(strlen(x) + strlen(y) + 1);
	strcpy(retval, x);
	strcat(retval, y);
	free(x);
	return retval;
}

char * get_syll_s(char * token, char ** moras, int * length) {
	
	int i = 0;
	
	for(;;) {
		
		char * mora = moras[i];
	
		/* If the token does not start with hiragana, then assume it's not a reading for the kanji. */
		if(!strcmp(mora, "")) return 0;
		
		/* Otherwise, read the first syllable in. */
		if(!strncmp(token, mora, strlen(mora))) { 
			*length += strlen(mora);
			return latin[i];
		} else {
			i++;
		}
	}
}

char * get_moras(char * word, char ** moras, int code) {
	char * morae = strdup("");
	char * mora = 0;
	int offset = 0;
	
	while((mora = get_syll_s(word + offset, hiragana, &offset))) {
		if(code) morae = append_string(morae, "(call mora ");
		morae = append_string(morae, mora);
		if(code) morae = append_string(morae, ") ");
	}
	if(!strlen(morae)) {
		free(morae);
		return 0;
	} else {
		return morae;
	}
}

char * transliterate(char * reading) {
	if(!strlen(reading)) return 0;
	char * retval = 0;
	retval = get_moras(reading, hiragana, 0);
	if(retval) return retval;
	retval = get_moras(reading, katakana, 0);
	return retval;
}

char * call_mora(char * reading) {
	return get_moras(reading, hiragana, 1);
}

void read_readings(reading * r, char * line) {

	r->moras = 0;
	r->moracode = 0;
	r->next = 0;

	char * token = line;
	while(strstr(token, " ")) {
		token = strstr(token, " ") + strlen(" ");
		char * m = transliterate(token);
		if(m) {
			
			r->moras = m;
			r->moracode = call_mora(token);
			
			r->next = malloc(sizeof(reading));
			r = r->next;

			r->moras = 0;
			r->moracode = 0;
			r->next = 0;
		}
	}
	return;
}

kanji * readkanjidic(FILE * kanjidic) {
	
	char * glyph = readword(kanjidic);
	if(!glyph || !strlen(glyph)) {
		free(glyph);
		return 0;
	}
	
	fprintf(stderr, "KANJIDIC: %s\r", glyph);
	
	char * jis = readword(kanjidic);
	if(!jis || !strlen(jis)) {
		free(glyph);
		free(jis);
		return 0;
	}
	
	kanji * kstruct = malloc(sizeof(kanji));
	kstruct->glyph = glyph;
	kstruct->jiscode = jis;
	kstruct->jouyou = 0;
	
	/* This contains a zero. Later on in the program, we will parse the
	 * EDICT dictionary for words that use these kanji. If a particular
	 * kanji is found in one of those words,  it gets marked by putting
	 * a one in here. Only kanji that are marked in this way will be
	 * output at the end. 
	 */
	kstruct->used = 0;
	
	char * line = readline(kanjidic);
	
	/* Find the Jouyou grade tag, and assign the appropriate value to the field
	 * in the struct. */
	     if(strstr(line, " G1 ")) kstruct->jouyou = 1;
	else if(strstr(line, " G2 ")) kstruct->jouyou = 2;
	else if(strstr(line, " G3 ")) kstruct->jouyou = 3;
	else if(strstr(line, " G4 ")) kstruct->jouyou = 4;
	else if(strstr(line, " G5 ")) kstruct->jouyou = 5;
	else if(strstr(line, " G6 ")) kstruct->jouyou = 6;
	
	kstruct->r = malloc(sizeof(reading));
	kstruct->r->moras = 0;
	kstruct->r->moracode = 0;
	kstruct->r->next = 0;
	
	/* Initialise this variable. This is used by the edict routines to
	 * mark a kanji as being used by some dictionary entry. */
	kstruct->used = 0;
	
	read_readings(kstruct->r, line);
	free(line);
	
	kstruct->next = readkanjidic(kanjidic);
	
	return kstruct;
}

int compilekanji(kanji * klist) {
	while(klist) {
		/* Skip any kanji that were not marked as used when parsing 
		 * the EDICT file. 
		 * Also skip any kanji that does not have an associated glyph
		 * or readings */
//		if(!klist->glyph || !klist->r || !klist->used) {
		if(!klist->glyph || !klist->r ) {
			klist = klist->next;
			continue;
		}
	
		printf("; GLYPH: %s; JIS %s\n", klist->glyph, klist->jiscode);
		
		/* For each reading, */
		reading * r = klist->r;
		while(r) {
			if(!r->moras) {
				r = r->next;
				continue;
			}
			
			/* Create a rule that matches the kanji */
			printf("(segment ");
			printf("(underlying %s-%s) ", klist->jiscode, r->moras);
			printf("(call kyouiku %d) ", klist->jouyou);
			printf("(surface %s))\n", klist->glyph);
			
			/* And one to match the hiragana/romaji */
			printf("(segment ");
			printf("(underlying %s-%s) ", klist->jiscode, r->moras);
			printf("(call kyouiku -%d) ", klist->jouyou);
			printf("%s)\n", r->moracode);
			
			r = r->next;
		}
		klist = klist->next;
	}
	return 0;
}

void free_reading(reading * r) {
	if(!r) return;
	free(r->moracode);
	free(r->moras);
	free_reading(r->next);
	free(r);
	return;
}

void free_kanji(kanji * klist) {
	if(!klist) return;
	
	if(klist->glyph) free(klist->glyph);
	if(klist->jiscode) free(klist->jiscode);
	if(klist->r) free_reading(klist->r);
	
	free_kanji(klist->next);
	free(klist);
	return;
}

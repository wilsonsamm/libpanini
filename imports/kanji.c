#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "read.h"

/* 
 * This program reads in all the kanji in the kanjidic.locale file. This is 
 * automatically adapted (see Makefile) from the kanjidic file prepared by Jim
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
 * This program reads in KANJIDIC and spits out Panini source code (the 
 * functional equivalent of (mostly) the same data, but usable by
 * libpanini.
 */

static char * hiragana[] = {
	"あ", "い", "う", "え", "お",
	"か", "き", "く", "け", "こ",
	"が", "ぎ", "ぐ", "げ", "ご",
	"さ", "し", "す", "せ", "そ",
	"ざ", "じ", "ず", "ぜ", "ぞ",
	"た", "ち", "つ", "て", "と",
	"だ", "ぢ", "づ", "で", "ど",
	"は", "ひ", "ふ", "へ", "ほ",
	"ば", "び", "ぶ", "べ", "ぼ",
	"ぱ", "ぴ", "ぷ", "ぺ", "ぽ",
	"ま", "み", "む", "め", "も",
	"な", "に", "ぬ", "ね", "の",
	"ら", "り", "る", "れ", "ろ",
	"や",       "ゆ",      "よ",
	"わ", "を", "ん",
	""
};

static char * latin[] = {
	 "a",  "i",  "u",  "e",  "o",
	"ka", "ki", "ku", "ke", "ko",
	"ga", "gi", "gu", "ge", "go",
	"sa", "si", "su", "se", "so",
	"za", "zi", "zu", "ze", "zo",
	"ta", "ti", "tu", "te", "to",
	"da", "di", "du", "de", "do",
	"ha", "hi", "hu", "he", "ho",
	"ba", "bi", "bu", "be", "bo",
	"pa", "pi", "pu", "pe", "po",
	"ma", "mi", "mu", "me", "mo",
	"na", "ni", "nu", "ne", "no",
	"ra", "ri", "ru", "re", "ro",
	"ya",       "yu",       "yo",
	"wa", "wo", "n",
	""
};

char * append_string(char * x, char * y) {
	char * retval = malloc(strlen(x) + strlen(y) + 1);
	strcpy(retval, x);
	strcat(retval, y);
	free(x);
	return retval;
}

int makeentry(char * kanji, char * jis, char * token, char * jouyou, char * negjouyou) {
	int i = 0;
	int offset = 0;
	
	//printf("trying to make a reading from %s.\n", token);
	
	char * romaji = strdup(""); /* This will contain the reading in romaji. */
	char * kana   = strdup(""); /* This will contain the instructions to call the kana */
	char * def1   = strdup(""); /* This will contain the definition for the kanji. */
	char * def2   = strdup(""); /* This will contain the definition for the kanji. */
	char * learn  = strdup(""); /* This will contain the definition for the kanji. */
	for(;;) {
		/* If the token does not start with hiragana, then assume it's not a reading for the kanji. */
		if(!strcmp(hiragana[i], "")) return 0;
		
		/* Otherwise, read the hiragana in one by one. */
		if(!strncmp(token + offset, hiragana[i], strlen(hiragana[i]))) { 
			romaji = append_string(romaji, latin[i]);
			kana = append_string(kana, "(call mora ");
			kana = append_string(kana, latin[i]);
			kana = append_string(kana, ") ");
			offset += strlen(hiragana[i]);
			i = 0;
		} else {
			i++;
			if(!strcmp(hiragana[i], "")) break;
		}
			
	}
	
	/* If we didn't pick up any reading, then just return. */
	if(!strlen(romaji)) {
		free(romaji);
		free(kana);
		free(def1);
		free(def2);
		return 1;
	}
	
	/* Construct the definition that matches/outputs the kanji. */
	def1 = append_string(def1, "(segment (underlying ");
	def1 = append_string(def1, jis);
	def1 = append_string(def1, "-");
	def1 = append_string(def1, romaji);
	def1 = append_string(def1, ") ");
	
	/* Should have the kanji itself and the reading for flags. */
	def1 = append_string(def1, "(surface ");
	def1 = append_string(def1, kanji);
	def1 = append_string(def1, ") ");
	
	/* Should assert the jouyou grade */
	def1 = append_string(def1, "(call kyouiku ");
	def1 = append_string(def1, jouyou);
	def1 = append_string(def1, ") ");
	
	/* Construct the definition that matches/outputs the kana. */
	def2 = append_string(def2, "(segment (underlying ");
	def2 = append_string(def2, jis);
	def2 = append_string(def2, "-");
	def2 = append_string(def2, romaji);
	def2 = append_string(def2, ") ");
	
	/* Should assert the jouyou grade */
	def2 = append_string(def2, "(call kyouiku ");
	def2 = append_string(def2, negjouyou);
	def2 = append_string(def2, ") ");
	
	/* Should match/output the kanji */
	def2 = append_string(def2, kana);
	def2 = append_string(def2, ") ");
	
	/* Construct the routine for learning kanji. */
	learn = append_string(learn, "(df learnkanji (segment ");
	learn = append_string(learn, jis);
	learn = append_string(learn, "-");
	learn = append_string(learn, romaji);
	learn = append_string(learn, ") (addseg ");
	learn = append_string(learn, jis);
	learn = append_string(learn, "-");
	learn = append_string(learn, romaji);
	learn = append_string(learn, ") (adjunct learnkanji))");
	
	
	printf("%s\n", def1);
	printf("%s\n", def2);
	printf("%s\n", learn);
	
	return 0;
	
}

int readtokens(FILE * kanjidic, char * jis, char * kanji) {
	char * line = readline(kanjidic);
	
	char * jouyou = "";
	char * negjouyou = "";
	
	if(strstr(line, "G1 ")) {
		jouyou = "1";
		negjouyou = "-1";
	}
	
	else if(strstr(line, "G2 ")) {
		jouyou = "2";
		negjouyou = "-2";
	}
	
	else if(strstr(line, "G3 ")) {
		jouyou = "3";
		negjouyou = "-3";
	}
	
	else if(strstr(line, "G4 ")) {
		jouyou = "4";
		negjouyou = "-4";
	}
	
	else if(strstr(line, "G5 ")) {
		jouyou = "5";
		negjouyou = "-5";
	}
	
	else if(strstr(line, "G6 ")) {
		jouyou = "6";
		negjouyou = "-6";
	}

	if(!strlen(jouyou)) return 0;
	if(!strlen(negjouyou)) return 0;
	
	char * token = line;
	while(strstr(token, " ")) {
		token = strstr(token, " ") + strlen(" ");
		makeentry(kanji, jis, token, jouyou, negjouyou);
	}
	return 0;
}

int main(int argc, char * argv[]) {
	
	FILE * kanjidic = fopen("./kanjidic.locale", "r");
	
	/* The first line is only a comment */
	skip_line(kanjidic);
	
	/* Take 6355 lines from the file */
	int i = 0;
	for(;;) {
		
		/* Read in the kanji itself. */
		char * kanji = readword(kanjidic);
		if(!kanji) break;
		if(!strlen(kanji)) break;
		
		char * jis = readword(kanjidic);
		
		//printf("; KANJIDIC ENTRY %s, ON LINE %d, JIS CODE %s \n", kanji, i, jis);
		
		readtokens(kanjidic, jis, kanji);
		i++;
		
		free(kanji);
		free(jis);
	}
	return 0;
}

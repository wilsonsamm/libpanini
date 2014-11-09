#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "read.h"

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
	return get_moras(reading, hiragana, 0);
}

char * call_mora(char * reading) {
	return get_moras(reading, hiragana, 1);
}

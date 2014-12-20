#include <stdio.h>
#include <panini.h>
#include <string.h>


/* This function is like strcmp except it only looks as far as (char*)y goes */
int shortstrcmp(char * x, char * y) {
	int i;
	
	for(i = 0; y[i]; i++) {
		if(!x[i]) return y[i];
		if(x[i] != y[i]) return x[i] - y[i];
	}
	return 0;
}

int var_assert(monad * m, char * namespace, char * varname, char * value) {
	char instruction[2048];
	
	strcpy(instruction, "(");
	strcat(instruction, namespace);
	strcat(instruction, "(");
	strcat(instruction, varname);
	strcat(instruction, " ");
	strcat(instruction, value);
	strcat(instruction, "))");
	
	panini_parse(m, instruction, "", 0, 0, -1);
	
	return 0;
}

/* The english language option has these options:
 *   :british    British English
 *   :american   American English
 *   :yorkshire  Yorkshire English
 */
int languages_english(monad * m, char * language) {
	monad_rules(m, "english");
	if(strstr(language, ":british"))   var_assert(m, "language", "orthography", "british");
	if(strstr(language, ":british"))   var_assert(m, "language", "norm",        "british");
	if(strstr(language, ":american"))  var_assert(m, "language", "orthography", "american");
	if(strstr(language, ":american"))  var_assert(m, "language", "norm",        "american");
	if(strstr(language, ":yorkshire")) var_assert(m, "language", "region",      "yorkshire");
	
	return 0;
}

/* The japanese language option has these options:
 *   :nokanji    Do not use any Kanji at all.
 *   :romaji     Write everything in latin letters.
 *   :kyouiku    (takes a numerical argument) What level of Kanji are OK? For example "japanese:kyouiku=4"
 */
int languages_japanese(monad * m, char * language) {
	monad_rules(m, "japanese");
	if(strstr(language, ":kyouiku="))  var_assert(m, "language", "kyouiku", strstr(language, ":kyouiku=") + 9);
	if(strstr(language, ":romaji"))    panini_parse(m, "(call romaji)", "", 0, 0, -1);
	if(strstr(language, ":nokanji"))   panini_parse(m, "(language -kanji) (call hiragana)", "", 0, 0, -1);
	
	return 0;
}

/* The nahuatl language option has these options:
 *   :carochi    Use the Carochi orthography.
 *   :classical  Use the classical orthography.
 */
int languages_nahuatl(monad * m, char * language) {
	monad_rules(m, "nahuatl");
	if(strstr(language, ":carochi"))   panini_parse(m, "(language (orthography carochi))", "", 0, 0, -1);
	if(strstr(language, ":classical")) panini_parse(m, "(language (orthography classical))", "", 0, 0, -1);
	
	return 0;
}

/* The cree language option has these options:
 *   :plains    Plains Cree
 *   :woods     Woods Cree
 *   :moose     Moose Cree
 */
int languages_cree(monad * m, char * language) {
	monad_rules(m, "algonquian");
	if(strstr(language, ":plains"))  panini_parse(m, "(language (language cree) (dialect plains))", "", 0, 0, -1);
	if(strstr(language, ":woods"))   panini_parse(m, "(language (language cree) (dialect woods) )", "", 0, 0, -1);
	if(strstr(language, ":moose"))   panini_parse(m, "(language (language cree) (dialect moose) )", "", 0, 0, -1);
	
	return 0;
}

/* The Ojibwe language option does not have any dialect switches (yet)
 */
int languages_ojibwe(monad * m, char * language) {
	monad_rules(m, "algonquian");
	panini_parse(m, "(language (language ojibwe))", "", 0, 0, -1);
	return 0;
}

/* These languages does not have any dialect switches (yet)
 * spanish french ainu swahili czech
 */
int langs_misc(monad * m, char * language) {
	/* If it's not one of the miscellaneous languages we know,
	 * then just return */
	if(strcmp(language, "spanish") && \
	   strcmp(language, "french") && \
	   strcmp(language, "ainu") && \
	   strcmp(language, "swahili") && \
	   strcmp(language, "czech")) {
		   return 1;
	}

	/* Otherwise load that language */
	monad_rules(m, language);
	return 0;
}

int languages(monad * m, char * langname) {
	     if(!shortstrcmp(langname, "english"))  languages_english(m, langname);
	else if(!shortstrcmp(langname, "japanese")) languages_japanese(m, langname);
	else if(!shortstrcmp(langname, "nahuatl"))  languages_nahuatl(m, langname);
	else if(!shortstrcmp(langname, "cree"))     languages_cree(m, langname);
	else if(!shortstrcmp(langname, "ojibwe"))   languages_ojibwe(m, langname);
	else return langs_misc(m, langname);
	return 0;
}

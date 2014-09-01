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

int m_do(monad * m, char * instr) {
	
	monad_map(m, set_stack, instr, -1);
	monad_map(m, tranny_parse, (void*)0, -1);
	
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
	
	m_do(m, instruction);
	
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
	if(strstr(language, ":romaji"))    m_do(m, "(call romaji)");
	if(strstr(language, ":nokanji"))   m_do(m, "(language -kanji) (call hiragana)");
	
	return 0;
}

/* The swahili language option has no options (yet!)
 */
int languages_swahili(monad * m, char * language) {
	monad_rules(m, "swahili");
	return 0;
}

/* The ainu language option has no options (yet!)
 */
int languages_ainu(monad * m, char * language) {
	monad_rules(m, "ainu");
	return 0;
}

/* The czech language option has no options (yet!)
 */
int languages_czech(monad * m, char * language) {
	monad_rules(m, "czech");
	return 0;
}

/* The nahuatl language option has these options:
 *   :carochi    Use the Carochi orthography.
 *   :classical  Use the classical orthography.
 */
int languages_nahuatl(monad * m, char * language) {
	monad_rules(m, "nahuatl");
	if(strstr(language, ":carochi"))   m_do(m, "(language (orthography carochi))");
	if(strstr(language, ":classical")) m_do(m, "(language (orthography classical))");
	
	return 0;
}

/* The cree language option has these options:
 *   :plains    Plains Cree
 *   :woods     Woods Cree
 *   :moose     Moose Cree
 */
int languages_cree(monad * m, char * language) {
	monad_rules(m, "algonquian");
	m_do(m, "(language (language cree))");
	if(strstr(language, ":plains"))  m_do(m, "(language (language cree) (dialect plains))");
	if(strstr(language, ":woods"))   m_do(m, "(language (language cree) (dialect woods))");
	if(strstr(language, ":moose"))   m_do(m, "(language (language cree) (dialect moose))");
	
	return 0;
}

/* The Ojibwe language option does not have any dialect switches (yet)
 */
int languages_ojibwe(monad * m, char * language) {
	monad_rules(m, "algonquian");
	m_do(m, "(language (language ojibwe))");
	return 0;
}

int languages(monad * m, char * langname) {
	     if(!shortstrcmp(langname, "english"))  languages_english(m, langname);
	else if(!shortstrcmp(langname, "japanese")) languages_japanese(m, langname);
	else if(!shortstrcmp(langname, "swahili"))  languages_swahili(m, langname);
	else if(!shortstrcmp(langname, "nahuatl"))  languages_nahuatl(m, langname);
	else if(!shortstrcmp(langname, "ainu"))     languages_ainu(m, langname);
	else if(!shortstrcmp(langname, "czech"))    languages_czech(m, langname);
	else if(!shortstrcmp(langname, "cree"))     languages_cree(m, langname);
	else if(!shortstrcmp(langname, "ojibwe"))   languages_ojibwe(m, langname);
	else return 1;
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panini.h>

#define NOUN 1
#define VERB 2

#define MASC 1
#define FEM  2
#define NEUT 3

/* This program searches through a file downloaded by the program get-page.sh
 * and tries to find as many translations in that file as possible.
 *
 * All over this program you will find a variable called wt, of type FILE*. This
 * refers to the file we're searching. (so-called because it comes from
 * wiktionary).
 * Also there variables by the names polish, norwegian, chinese etc. These are
 * output files that can be parsed later on to learn the word(s).
 */

char line[2048];
char * en_lemma = 0;

FILE * english = 0;
FILE * czech = 0;

int nextline(FILE * wt, char * s) {
	int offs = 0;
	s[0] = '\0';
	int ch;
	ch = fgetc(wt);
	if(ch == EOF) return 0;
	
	while(ch != EOF && ch != '\n' && ch != '\r') {
		s[offs++] = ch;
		ch = fgetc(wt);
	}
	s[offs] = '\0';
	return 1;
}

int test(char * lang, char * exec, char * lex) {
//	char e[4096];
	int retval;
//	
//	strcpy(e, "(seme (head ");
//	strcat(e, seme);
//	strcat(e, ") ");
//	strcat(e, exec);
//	
//	fprintf(stderr, "Testing %s ...\r", e);
//
	monad * m = monad_new();
	monad_rules(m, lang);
//	
	retval = panini_parse(m, exec, lex, 0, 0, 10);
	monad_free(m);
//	fprintf(stderr, "Done testing.\n");
	return retval;
}

void englishlemma(int sense, int pos) {
	if(!english) {
		english = fopen("english.panini", "a+");
	}
	if(pos == NOUN) {
		fprintf(english, "(df nounstem (lit %s) ", en_lemma);
		fprintf(english, "(seme (head %d%s)) ", sense, en_lemma);
		fprintf(english, "(confidence -%d))\n", sense);
	}
}

void learn_noun(char * fn, char * exec, char * lang, char * word) {
	if (test(lang, exec, word)) return;
	FILE * outfile = fopen(fn, "a");

	monad * m = monad_new();
	monad_rules(m, lang);
	monad * n = monad_duplicate(m);
	if(!panini_parse(m, exec, word, 0, 0, 5)) {
//		printf("Going to learn the %s %s, %s ... \n", lang, "noun", word);
		panini_learnvocab(n, exec, outfile, word, 20);
	}
	fclose(outfile);
	monad_free(m);
	monad_free(n);
}

void mfn_translation(int sense, int pos, char * line, char * lang, char * shortcode, char * plang) {
	char word[1024];
	char itest[1024];
	char fname[1024];
	char seme[32];

	sprintf(seme, "%d%s", sense, en_lemma);
	sprintf(fname, "./%s.panini", plang);
	sprintf(itest, "/usr/panini/targets/%s", plang);
	/* Check that the language module is already installed. */
	FILE * tst = fopen(itest, "r");
	if(!tst) return;
	fclose(tst);

	/* Check that the line in the file really has the expected information.
	 * For example, for Latin, we'd expect the line to start like this:
	 * * Latin: {{t|la|ascia|f}}
	 */
	if(!strstr(line, "{{")) return;
	if(!strstr(strstr(line, "{{") + 2, "|")) return;
	if(strncmp(strstr(strstr(line, "{{") + 2, "|") + 1, shortcode, strlen(shortcode))) return;
	strcpy(word, strstr(strstr(strstr(line, "{{") + 2, "|") + 1, "|") + 1);
	strcat(word, " ");
	
	char exec[4096];
	strcpy(exec, "(seme (head ");
	strcat(exec, seme);
	strcat(exec, ")) ");
	
	if(pos == NOUN) {
		char * end;
		
		strcat(exec, "(call noun learn singular nominative ");
	
		if((end = strstr(word, "|f}}"))) {
			strcat(exec, "feminine");
			end[0] = '\0';
		}
		if((end = strstr(word, "|m}}"))) {
			strcat(exec, "masculine");
			end[0] = '\0';
		}
		if((end = strstr(word, "|n}}"))) {
			strcat(exec, "neuter");
			end[0] = '\0';
		}
	}
	if(strstr(word, "}")) return;
	strcat(exec, ") ");
	learn_noun(fname, exec, plang, word);
	
}

void untangle_chinese(int sense, int pos, char * buf, char * langcode) {
	char * buffer = buf;

	/* This is a mechanism that ensures that the function onyl runs to
	 * completion once each time the program is invoked.
	 */
	static int hasbeen = 0;
	if (hasbeen) return;
	
	char seme[32];
	snprintf(seme, 32, "%d%s", sense, en_lemma);
	
	char exec[4096];
	strcpy(exec, "(seme (head ");
	strcat(exec, seme);
	strcat(exec, ")) (language (language mandarin) (orthography simplified)) (call noun)");
	
	while(buffer) {
		char * endl;
		char expr[1024];
		char hanzi[1024];
//		char tr[1024];
		hanzi[0] = '\0';
//		tr[0] = '\0';
		if (hasbeen) return;
		
		strncpy(expr, strstr(buffer, "{{"), 1024);
		endl = strstr(expr, "}},");
		if(endl) endl[2] = '\0';
//		printf("untangle_chinese found %s\n", expr);
		buffer = strstr(buffer, "}}");
		buffer = strstr(buffer, "{{");

//		printf("langcode = %s\n", langcode);
		char * ht = strstr(expr, langcode);
		if(!ht) continue;
		strncpy(hanzi, ht + strlen(langcode), 1024);
		endl = strstr(hanzi, "|");
		if (endl) endl[0] = '\0';
//		printf("hanzi = %s\n", hanzi);

//		if(!strstr(expr, "tr=")) continue;
		
//		strncpy(tr, strstr(expr, "tr=") + 3, 1024);
//		endl = strstr(tr, "|");
//		if (endl) endl[0] = '\0';
//		endl = strstr(tr, "}}");
//		if (endl) endl[0] = '\0';
//		printf("tr = %s\n", tr);

//		if(!strlen(tr)) continue;
		if(!strlen(hanzi)) continue;

		learn_noun("chinese.panini", exec, "chinese", hanzi);
//		runtest("chinese.panini", exec, "chinese", hanzi);
		hasbeen = 1;
		
	}
}

void extract_ainu(int sense, int pos, char * buf) {

	char seme[32];
	snprintf(seme, 32, "%d%s", sense, en_lemma);
	
	char exec[4096];
	strcpy(exec, "(seme (head ");
	strcat(exec, seme);
	strcat(exec, ")) (call noun)");
	
	char tr[1024];
	char * nulterm;
	
	if(!strstr(buf, "|tr=")) return;

	strcpy(tr, strstr(buf, "|tr=") + 4);
	nulterm = strstr(tr, "|");
	if(nulterm) nulterm[0] = '\0';
	nulterm = strstr(tr, "}}");
	if(nulterm) nulterm[0] = '\0';

	learn_noun("ainu.panini", exec, "ainu", tr);
}

void translate_from_english(FILE * wt) {
	rewind(wt);
	char buffer[9999];
	char def[9999];
	int pos = 0;
	int sense = 0;
	int cont = 1;
	while(cont) {
		cont = nextline(wt, buffer);
//		fprintf(stderr, "'%s'\n", buffer);
	
		if(!strcmp(buffer, "====Noun====")) {
//			fprintf(stderr, "; Noun.\n");
			pos = NOUN;
			continue;
		}
		if(!strcmp(buffer, "====Verb====")) {
//			fprintf(stderr, "; Noun.\n");
			pos = VERB;
			continue;
		}
		
		if(!strncmp(buffer, "{{trans-top|", 12)) {
			sense++;
			strcpy(def, buffer + 12);
			char * end = strstr(def, "}}");
			end[0] = '\0';
//			fprintf(stderr, "; sense number %d: %s\n", sense, def);
			englishlemma(pos, sense);
			continue;
		}

		
		if(!strncmp(buffer, "* Czech: ", 9)) {
			mfn_translation(sense, pos, buffer, "Czech", "cs", "czech");
			continue;
		}
		
		if(!strncmp(buffer, "* Latin: ", 9)) {
			mfn_translation(sense, pos, buffer, "Latin", "la", "latin");
			continue;
		}
		
		if(!strncmp(buffer, "* Ainu: ", 8)) {
			extract_ainu(sense, pos, buffer);
			continue;
		}
		
//		if(!strncmp(buffer, "*: Mandarin: ", 13)) {
//			untangle_chinese(sense, pos, buffer, "|cmn|");
//			continue;
//		}
	}
}

int main(int argc, char * argv[]) {

	FILE * wt = 0;
	
	int i;
	for(i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-s")) {
			i++;
			wt = fopen(argv[i], "r+");
			continue;
		} else if(!strcmp(argv[i], "-l")) {
			i++;
			en_lemma = argv[i++];
			continue;
		} else {
			fprintf(stderr,"what does \"%s\" mean.\n", argv[i]);
			exit(1);
		}
	}

	translate_from_english(wt);
	
	return 0;
}

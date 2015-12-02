#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panini.h>
//#include <unistd.h>

int noun_test(char * english, char * pinyin, char * traditional, char * simplified) {
	int retval;
	/* create a new monad. */
	monad * m = monad_new();

	/* Try to understand the english. */
	monad_rules(m, "english");
	retval = panini_parse(m, "(fork ((call noun)) ((call nounphrase))) (lit .)", english, 0, 0, 5);
	
	/* If the English parse wasn't successful, then we can not determine that
	 * the Chinese is missing. */
	if(!retval) {
		if(!strstr(english, " ")) {
			FILE * en = fopen("eng.txt", "a+");
			char * s = strstr(english, ".");
			if (s) s[0] = '\0';
			fprintf(en, "%s\n", english);
			fclose(en);
		}
		monad_free(m);
		return 0;
	}

	monad_rules(m, "chinese");
	retval = panini_parse(m, "(language (orthography traditional)) (call noun) (lit .)", traditional, 0, 1, 5);
	if(!retval) {
		monad_free(m);
		return 1;
	}

	monad_map(m, remove_ns, "language", -1);
	retval = panini_parse(m, "(language (orthography simplified)) (recorded-segments) (lit .)", simplified, 0, 1, 5);
	if(!retval) {
		monad_free(m);
		return 1;
	}
	monad_map(m, remove_ns, "language", -1);
	retval = panini_parse(m, "(language (orthography pinyin2)) (recorded-segments) (lit .)", pinyin, 0, 1, 5);
	if(!retval) {
		monad_free(m);
		return 1;
	}
	printf("I already know that %s %s %s means %s.\n", traditional, simplified, pinyin, english);
	monad_free(m);
	return 1;
}

int noun_learn(char * english, char * pinyin, char * traditional, char * simplified) {
	int retval;
	monad * m = monad_new();

	/* Try to understand the english. */
	monad_rules(m, "english");
	retval = panini_parse(m, "(fork ((call noun)) ((call nounphrase))) (lit .)", english, 0, 0, 5);
		
	/* If the parse wasn't successful, then exit. */
	if(!retval) {
		monad_free(m);
		return 0;
	}
	printf("learning %s ", english);
	fflush(stdout);

	monad_rules(m, "chinese");
	retval = panini_parse(m, "(language (orthography traditional)) (call guess-syllables) (lit .)", traditional, 0, 1, 5);
	if(!retval) {
		printf("%s ERROR\n", traditional);
		monad_free(m);
		return 1;
	}
	printf("%s ", traditional);
	fflush(stdout);

	monad_map(m, remove_ns, "language", -1);
	retval = panini_parse(m, "(language (orthography simplified)) (recorded-segments) (lit .)", simplified, 0, 1, 5);
	if(!retval) {
		printf("%s ERROR\n", simplified);
		monad_free(m);
		return 1;
	}
	printf("%s ", simplified);
	fflush(stdout);

	monad_map(m, remove_ns, "language", -1);
	retval = panini_parse(m, "(language (orthography pinyin2)) (recorded-segments) (lit .)", pinyin, 0, 1, 5);
	if(!retval) {
		printf("%s ERROR\n", pinyin);
		monad_free(m);
		return 1;
	}
	printf("%s ", pinyin);
	fflush(stdout);

	FILE * chinese_out = fopen("chinese.panini", "a+");
	panini_learnvocab(m, "(open noun) (add-record-to-df noun) (add-ns-to-df noun seme head)	(add-ns-to-df noun language language)", chinese_out, "", 5);
	fclose(chinese_out);
	printf(" OK \n");
	return 1;
}

int main(int argc, char * argv[]) {

	char * en   = argv[1];
	char * py   = argv[2];
	char * trad = argv[3];
	char * simp = argv[4];
	
	if(!noun_test(en, py, trad, simp))
		if(noun_learn(en, py, trad, simp))
			return 0;
	return 1;
}

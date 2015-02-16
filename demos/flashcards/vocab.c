#include "flash.h"

int vocab(char * category, char * lang1, char * lang2) {
	
	if(!category || !lang1 || !lang2) {
		printf("Usage:\n $ flashcards vocab [CATEGORY] [LANGUAGE1] ");
		printf("[LANGUAGE2]\n");
		printf("possible languages include czech, ojibwe, ...\n");
		printf("possible categories include month, people, ...\n");
		return 1;
	}
	
	monad * m = monad_new();
	monad_rules(m, "world");
	
	char exec[1024];
	snprintf(exec, 1024, "(call relation %s)", category);
	panini_generate(m, exec, 0, 5);
	
	monad_rules(m, lang1);
	panini_generate(m, "(lit \")(call lemma)(lit \",)", 0, 5);
	
	monad_rules(m, lang2);
	panini_generate(m, "(lit \")(debug)(call lemma)(lit \")", 0, 5);
	
	FILE * vout = fopen("vocab.csv", "w+");
	monad_map(m, print_out, vout, 5);
	
	monad_free(m);
	return 0;
}

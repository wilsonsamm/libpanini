#include "../monad/monad.h"
#include "../list/list.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

#define BUFLEN 1024

void tidyup(char * string) {
	int i;
	
	for(i = 0; i < strlen(string); i++) {
		if(string[i] == '\n' || string[i] == ' ' ) {
			string[i] = '\0';
			return;
		}
	}
}

/* This program asks the user for input, and then gets Tranny to take over to learn the word. */
void prompt(monad * m, char * target, FILE * fp) {
	monad_map(m, strp, (void *)0, -1);
	char input[BUFLEN];
	
	fgets(input, BUFLEN, stdin);
	if(!strlen(input)) return;
	
	tidyup(input);
	strcat(input, ".");
	
	monad * n = monad_duplicate(m);
	monad_rules(n, target);
	
	monad_map(n, (int(*)(monad *, void *))remove_ns, "rection", -1);
	monad_map(n, (int(*)(monad *, void *))set_intext, input, -1);
	monad_map(n, (int(*)(monad *, void *))set_stack, "(debug) (call Learn lemma)", -1);
	monad_map(n, (int(*)(monad *, void *))tranny_learn, (void *)0, 5);
	monad_map(n, (int(*)(monad *, void *))print_out, fp, 5);	
	fflush(fp);
}

/* This function tests if a word if translatable */
int test(monad * m, char * target) {
	int retval;
	
	monad * n = monad_duplicate(m);
	
	monad_rules(n, target);
	
	monad_map(n, (int (*)(monad *, void *))set_intext, m->intext, -1);
	
	monad_map(n, (int (*)(monad *, void *))set_stack, "(call Translation)", -1);

	retval = monad_map(n, (int (*)(monad *, void *))tranny_generate, (void *)0, 20);
	
	if(retval) printf("There is already a word in %s for \"%s\".\n", target, n->intext);
	else {
		printf("I don't know a word in %s for \"%s\". Do you? ", target, n->intext);
	}
	
	monad_free(n);
	
	return !retval;
	
}

void test_each(monad * m, char * target) {
	/* Strip the whitespace off the INTEXT and the OUTTEXT */
	monad_map(m, strp, (void *)0, -1);
	
	char * fn = malloc(strlen(target) + strlen(LEARNPATH) +1);
	strcpy(fn, LEARNPATH);
	strcat(fn, target);
	
	FILE * fp = fopen(fn, "a+w");
	
	if(!fp) {
		printf("Could not open %s.\n", fn);
		free(fn);
		return;
	}
	free(fn);
	
	/* Test each monad. */
	while(m) {
		if(m->alive == 0) {
			m = m->child;
			continue;
		}
		
		/* If the word could not be translated, then we'll prompt the user. */
		if(test(m, target)) prompt(m, target, fp);
		
		m = m->child;
	}
}

int main(int argc, char * argv[]) {
	
	if(argc < 2) {
		printf("\nHere's how to use this program:\n $ ./interrogator swahili english\n");
		printf("This will look at all Swahili words for which there is no known English\n");
		printf("translation. Then the user will be prompted for the word and the appropriate\n");
		printf("Tranny source code will be generated to match that word to that meaning.\n");
		printf("\nThis program is intended as a quick and easy way to expand the vocabulary of \n");
		printf("the language you want. It is not intended as a jenuine machine learning program.\n");
		printf("\nNext time you recompile the language rules, the generated stuff will be picked");
		printf("up and installed.\nAnd of course, you can substitute \"english\" or \"swahili\" for anything else, but\n");
		printf("the prerequisite is that the source language (Swahili in the example above) has \n");
		printf("working \"Headwords\" rules, and the target language (English in the example above)\n");
		printf("has working \"Learn\" rules.\n");
		printf("Have fun!\n\n");
		
		exit(0);
	}
	
	monad * m = headwords(argv[1]);
	
	test_each(m, argv[2]);
	return 0;
}

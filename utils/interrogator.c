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
			string[i] = 0;
			return;
		}
	}
}

/* This program asks the user for input, and then gets Tranny to take over to learn the word. */
void prompt(monad * m, char * target) {
	monad_map(m, strp, (void *)0, -1);
	char input[BUFLEN];
	
	fgets(input, BUFLEN, stdin);
	
	tidyup(input);
	
	if(!strlen(input)) return;
	
	monad * n = monad_duplicate(m);
	monad_rules(n, target);
	
	monad_map(n, (int(*)(monad *, void *))set_intext, input, -1);
	monad_map(n, (int(*)(monad *, void *))set_stack, "(constituent Learn)", -1);
	monad_map(n, (int(*)(monad *, void *))tranny_learn, (void *)0, 5);
	monad_map(n, (int(*)(monad *, void *))print_out, stderr, 5);
	
}

/* This function tests if a word if translatable */
int test(monad * m, char * target) {
	int retval;
	
	monad * n = monad_duplicate(m);
	
	monad_rules(n, target);
	
	monad_map(n, (int (*)(monad *, void *))set_intext, m->intext, -1);
	
	monad_map(n, (int (*)(monad *, void *))set_stack, "(constituent Headword)", -1);

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

	/* Test each monad. */
	while(m) {
		if(m->alive == 0) {
			m = m->child;
			continue;
		}
		
		/* If the word could not be translated, then we'll prompt the user. */
		if(test(m, target)) prompt(m, target);
		
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
		printf("\nAlso, remember to pipe stderr to /usr/tranny/learned/english. Next time you\n");
		printf("recompile the language rules, the generated stuff will be picked up and installed.\n");
		printf("And of course, you can substitute \"english\" or \"swahili\" for anything else, but\n");
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

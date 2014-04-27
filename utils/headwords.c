#include "../monad/monad.h"
#include "utils.h"
#include <string.h>

int movetxt(monad * m, void * nothing) {
	m->intext = m->outtext;
	m->outtext = 0;
	return 0;
}

int kill_identical_intexts(monad * m, void * nothing) {
	int kio(monad * m, void * n) {
		if(!m->intext) return 0;
		if(!strcmp((char*)n, m->intext)) m->alive = 0;
		return 0;
	}
	
	if(!m->intext) return 0;
	
	monad_map(m->child, kio, m->intext, -1);
	return 0;
}

monad * headwords(char * language) {
	
	/* Get a new monad */
	monad * m = monad_new();
	
	/* In the given language, */
	monad_rules(m, language);
	
	/* Generate all the lemmas. */
	monad_map(m, (int(*)(monad *, void *))set_stack, "(constituent Headword)", -1);
	
	monad_map(m, tranny_generate, language, -1);
	monad_map(m, kill_identical_outtexts, (void *)0, -1);
	
	/* Remember the headword in the INTEXT register */
	monad_map(m, movetxt, (void *)0, -1);
	
	/* Next, generate the meaning of the headwords */
	monad_map(m, (int(*)(monad *, void *))set_stack, "(constituent Headword)", -1);
	monad_map(m, tranny_parse, language, -1);
	monad_map(m, kill_identical_intexts, (void *)0, -1);
	
	/* Get rid all all unnecessary monads */
	monad_map(m, unlink_the_dead, (void *)0, -1);
	
	return m;
}

monad * words(char * language) {
	
	/* Get a new monad */
	monad * m = monad_new();
	
	/* In the given language, */
	monad_rules(m, language);
	
	/* Generate all the lemmas. */
	monad_map(m, (int(*)(monad *, void *))set_stack, "(constituent Word)", -1);
	
	monad_map(m, tranny_generate, language, -1);
	monad_map(m, kill_identical_outtexts, (void *)0, -1);
	
	return m;
}

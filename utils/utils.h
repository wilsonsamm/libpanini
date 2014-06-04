#include "../monad/monad.h"

#define LEARNPATH "/usr/tranny/learned/"

/* Given the name of a language, generate all the headwords in a chain of monads and return it. */
monad * headwords(char * language);
monad * words(char * language);

/* This function strips whitespace off the end of the string you pass it */
void strip(char * c);
int strp(monad * m, void * nothing);

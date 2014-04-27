#include "../monad/monad.h"
#include "../list/list.h"

#include "utils.h"

/* This program produces a list of all the defined words in the 
 * language. (you can feed it through a spellchecker for example). */

int main(int argc, char * argv[]) {
	
	monad * m = words(argv[1]);
	monad_map(m, (int(*)(monad *, void *))print_out, stdout, -1);
	
	return 0;
}

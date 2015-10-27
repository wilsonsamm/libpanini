#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panini.h>

int main(int argc, char * argv[]) {

	monad * m = panini_test(argv[1], argv[2], argv[3], argv[4], argv[5]);
	if(m) {
		monad_free(m);
		exit(1);
	} else {
		exit(0);
	}
}

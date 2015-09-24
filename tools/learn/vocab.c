
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../panini.h"
#include "readfile.h"

int main(int argc, char * argv[]) {

	monad * m = panini_test(argv[1], argv[2], argv[3], argv[4], argv[5]);
	if(!m) {
		fprintf(stderr, "Nothing returned from panini_test.\nExiting.\n");
		monad_free(m);
		exit(2);
	}

	fprintf(stderr, "panini_test returned a monad, gon try and learn.\n");
	
	panini_learnvocab(m, argv[5], stdout, argv[6], 5);
	return 0;
}

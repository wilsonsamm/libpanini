#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wn.h>
#include "../../panini.h"

int main(int argc, char * argv[], char * envp[]) {

	/* Initialise WordNet */
	wninit();

	int pos = -1;
	if(!strcmp("noun", argv[1])) pos = NOUN;
	if(pos == -1) {
		printf("unknown part-of-speech %s\n", argv[1]);
		exit(99);
	}
	
	SynsetPtr result_ds = findtheinfo_ds(argv[2], pos, OVERVIEW, ALLSENSES);

	int i = 0;
	while(result_ds) {
		
		printf("; %s\n", result_ds->defn);
		int j;
		for(j = 0; j < result_ds->wcount; j++) {
			printf("(df nounstem (seme (head %d%s)) ", i, argv[2]);
			printf("(lit %s))\n", result_ds->words[j]);
		}

		result_ds = result_ds->nextss;
		printf("\n");
		i++;
	}
	
	return 0;
}

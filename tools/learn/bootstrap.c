
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../panini.h"
#include "readfile.h"

int parsesection(FILE * fp) {

	FILE * outfile = fopen("bootstrap.out", "a");
	
	nextline(fp);
	if(!currentline) return 0;
	if(!strlen(currentline)) return 1;
	
	/* get the code from the first line in the section.
	 * This is appended to the string "bootstrap-" and then this is called.
	 * For example, if the first line reads "nounstem none" then for each word
	 * in that section, this program will do (call bootstrap-nounstem none).
	 */
	char * boot = getfield(1);
		
	/* Let's keep the user up-to-date with what's happening. */
	progpc(NONE, 0);
	
	nextline(fp);
	while(strlen(currentline)) {
		
		char * btext = getfield(1);
		char * atext = getfield(2);
		if(!atext || !strlen(atext)) atext = strdup(btext);
		
		monad * m = monad_new();
		monad_rules(m, "./english");

		char * bexec = malloc(2048);
		strcpy(bexec, "(language (norm british)) (seme (head ");
		strcat(bexec, btext);
		strcat(bexec, "))(call bootstrap-");
		strcat(bexec, boot);
		strcat(bexec, ")");

		char * aexec = malloc(80);
		strcpy(aexec, "(language (norm american)) (recorded-segments)");

		/* Generate all possible segmentations of the word, in British and
		 * American spellings */
		if(!panini_learnvocab(m, bexec, 0, btext, 10)) {
			progpc(SAD, 0);
		} else {
		/* And then remove those which are not valid for American spelling. */
			monad_map(m, remove_ns, "language", -1);
			panini_parse(m, aexec, atext, 0, 1, 10);
			monad_map(m, print_out, outfile, -1);
			progpc(HAPPY, 0);
		}

		free(atext);
		free(btext);
		nextline(fp);
		monad_free(m);
		
		if(!currentline || strlen(currentline) == 1) {
			break;
		}
		
		free(aexec);
		free(bexec);
	}
	fclose(outfile);
	free(boot);
	progpc(NONE, 0);
	return 1;
}

int main(int argc, char * argv[]) {

	/* Open the textfile that has all the sentences in it. */
	FILE * fp = fopen(argv[1], "r");
	if(!fp) {
		fprintf(stderr,"Cannot open %s.\n", argv[1]);
		exit(1);
	}
	
	count_lines(fp);
	
	while(parsesection(fp));
	printf("\n");
	
	progpc(DONE, 0);
	fclose(fp);
	return 0;
}

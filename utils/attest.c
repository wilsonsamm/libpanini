#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ATTESTDIR "/usr/tranny/attested/"

char * readfile(FILE *fp) {
	int alloc = 1024;
	int readin = 0;
	char * f = malloc(alloc);
	f[0] = '\0';
	
	for( ; ; ) {
		char ch = fgetc(fp);
		
		if(ch == EOF) {
			f[readin] = '\0';
			break;
		}
		
		f[readin] = (char) ch;
		readin++;
		if(readin > alloc - 2) {
			alloc += 1024;
			f = realloc(f, alloc);
		}
	}

	return f;
}

int main(int argc, char * argv[]) {
	
	/* create a new monad. */
	monad * m = monad_new();

	/* Check for command line sanity. */
	if(argc < 3) {
		printf("This program exists because the learner program(s) like \"interrogator\" here can\n");
		printf("be a bit promiscuous and pick up a lot of garbage among the good things. The\n");
		printf("thought is that if a rule is actually used in parsing a sentence, then it is\n");
		printf("likely to be a good rule. So it will be \"attested\", and then next time you\n");
		printf("recompile tranny, then all attested rules may be used in parsing or generating\n");
		printf("even unseen text.\n\n");
		printf("To use this program, type:\n");
		printf(" $ ./attest language filename\n");
		printf("where filename is the path to the file containing the data you want to have \nanalysed, \n");
		printf("and language is the name of the language that the file is in.\n");
		
		return 1;
	}
	
	/* Read the file and set the INTEXT register in the monad. */
	FILE * fp = fopen(argv[2], "r");
	char * text = readfile(fp);
	monad_map(m, set_intext, (void *)text, -1);
	
	/* Load the language rules */
	monad_rules(m, argv[1]);
	
	/* Parse! */
	monad_map(m, set_stack, "(constituent main)", -1);
	monad_map(m, tranny_attest, argv[1], 5);
	monad_map(m, kill_not_done, (void*)0, -1);
	monad_map(m, kill_least_confident, (void*)0, -1);
	
	/* open the attest file and print everything out there. */
	char * fn = malloc(strlen(ATTESTDIR) + strlen(argv[1]) +2);
	strcpy(fn, ATTESTDIR);
	strcat(fn, argv[1]);
	FILE * attest = fopen(fn, "a");
	
	monad_map(m, print_out, attest, -1);
	
	
	monad_free(m);
	free(fn);
	free(text);
	fclose(fp);
	fclose(attest);
	return 0;
}

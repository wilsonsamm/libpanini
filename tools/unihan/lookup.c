#include <stdio.h>

int main(int argc, char * argv[]) {
	FILE * infile = fopen(argv[1], "r");
	if(!infile) {
		fprintf(stderr,"Could not open %s.\nStop.\n", argv[1]);
		exit(99);
	}

	char codepoint[1024];
	char key[1024];
	char value[1024];
	int c = fgetc(infile);
	int offset;
	
linestart:
	if(c == EOF) exit(0);
	
	/* skip all comments */
	if(c == '#') {
		while(c != '\n') c = fgetc(infile);
		c = fgetc(infile);
		if(c == EOF) exit(0);
		goto linestart;
	}

	offset = 0;
	/* Read the codepoint in */
	while(c != '\t') {
		if(c == EOF) exit(0);
		codepoint[offset++] = c;
		c = fgetc(infile);
	}
	codepoint[offset] = '\0';

	/* Skip any whitespace */
	c = fgetc(infile);

	offset = 0;
	key[offset] = c;
	/* Read the key in */
	while(c != '\t') {
		key[offset++] = c;
		c = fgetc(infile);
	}
	key[offset] = '\0';

	/* Skip any whitespace */

	c = fgetc(infile);
	/* Read the value in */
	offset = 0;
	value[offset] = c;
	/* Read the key in */
	while(c != '\n') {
		value[offset++] = c;
		c = fgetc(infile);
	}
	value[offset] = '\0';

	if(!strcmp(key, argv[2])) {
		printf("%s %s\n", codepoint, value);
	}

	/* End of line */
	c = fgetc(infile);
	goto linestart;
}

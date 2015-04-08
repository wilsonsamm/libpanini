#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void def_pinyin(char * codepoint, char * pinyin) {
	printf("(df codepoint (flags %s pinyin) (lit %s) ", codepoint, pinyin);
	printf("(language (language mandarin) (orthography pinyin)))\n");
}

void def_jyutping(char * codepoint, char * jyutping) {
	if(strstr(jyutping, " ")) return;
	printf("(df codepoint (flags %s jyutping) (lit %s) ", codepoint, jyutping);
	printf("(language (language cantonese) (orthography jyutping)))\n");
}

void def_simplified(char * codepoint, char * character) {
	printf("(df codepoint (flags %s simplified) (lit %s) ", codepoint, character);
	printf("(language (orthography simplified)))\n");
}

void def_traditional(char * codepoint, char * character) {
	printf("(df codepoint (flags %s simplified) (lit %s) ", codepoint, character);
	printf("(language (orthography traditional)))\n");
}

void generate_utf8(char * codepoint) {
	char chr[8];
	
	int v = (int)strtol(codepoint + 2, NULL, 16);
	if(!v) return;

	/* If the codepoint is in the range 0x0800-0xFFFF, then: */
	if(v > 127 && v < 65536) {
		
		/* The first byte should be 0xE0 orred with the first four bits */
		chr[0] = (char)(224 | (( v >> 12) & 15));

		/* Two continuation bytes */
		chr[1] = (char)(128 | (( v >>  6) & 63));
		chr[2] = (char)(128 | (  v        & 63));

		/* and null termination */
		chr[3] = 0;
		printf("%s %s\n",codepoint, chr);
	} else {
	//	fprintf(stderr,"No UTF-8 for codepoint %s.\n", codepoint);
	}
	
}

int main(int argc, char * argv[]) {
	
	if(!stdin) {
		fprintf(stderr,"Could not open stdin.\nStop.\n");
		exit(99);
	}

	char codepoint[1024];
	char value[1024];
	int offset = 0;
	
linestart:
	offset = 0;
	int c = fgetc(stdin);
	
	if(c == EOF) exit(0);

	/* Read the codepoint in */
	offset = 0;
	while(c != ' ') {
		if(c == EOF) exit(0);
		codepoint[offset++] = c;
		c = fgetc(stdin);
	}
	codepoint[offset] = '\0';

	/* Skip any whitespace */
	c = fgetc(stdin);

	offset = 0;
	value[offset] = c;
	/* Read the key in */
	while(c != '\n') {
		value[offset++] = c;
		c = fgetc(stdin);
	}
	value[offset] = '\0';

	if(!strcmp(argv[1], "pinyin"))      def_pinyin     (codepoint, value);
	if(!strcmp(argv[1], "jyutping"))    def_jyutping   (codepoint, value);
	if(!strcmp(argv[1], "simplified"))  def_simplified (codepoint, value);
	if(!strcmp(argv[1], "traditional")) def_traditional(codepoint, value);
	if(!strcmp(argv[1], "utf8"))     generate_utf8(codepoint);

	/* End of line */
	goto linestart;
}

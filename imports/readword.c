#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define INITALLOC 20	// How many bytes to allocate initially
#define INCALLOC   8	// How many bytes to increment the allocation by


char * readword(FILE *fp) {
	int c = fgetc(fp);
	
	while(isspace(c) && c != EOF) c = fgetc(fp);
	
	int allocated = INITALLOC;
	char * word = malloc(INITALLOC);
	int i = 0;
		
	while(!isspace(c) && c != EOF) {
		word[i++] = (char)c;
		if(i > allocated - 2) {
			allocated += INCALLOC;
			word = realloc(word, allocated);
		}
		c = fgetc(fp);
	}
	word[i] = '\0';
	return word;
}

char * readline(FILE *fp) {
	int c = fgetc(fp);
	
	int allocated = INITALLOC;
	char * line = malloc(INITALLOC);
	int i = 0;
		
	while(c != '\n' && c != '\f' && c != '\r') {
		line[i++] = (char)c;
		if(i > allocated - 2) {
			allocated += INCALLOC;
			line = realloc(line, allocated);
		}
		c = fgetc(fp);
	}
	line[i] = '\0';
	return line;
}

int skip_word(FILE * fp) {
	int c = fgetc(fp);
	while(!isspace(c)) c = fgetc(fp);
}

int skip_line(FILE * fp) {
	int c = fgetc(fp);
	while(c != '\n' && c != '\r' && c != '\f') c = fgetc(fp);
}
	

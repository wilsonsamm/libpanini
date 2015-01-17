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
		
	while(c != '\n' && c != '\f' && c != '\r' && c != EOF) {
		line[i++] = (char)c;
		if(i > allocated - 2) {
			allocated += INCALLOC;
			line = realloc(line, allocated);
		}
		line[i] = '\0';
		c = fgetc(fp);
	}
	line[i] = '\0';
	return line;
}

void skip_word(FILE * fp) {
	int c = fgetc(fp);
	while(!isspace(c)) c = fgetc(fp);
}

void skip_line(FILE * fp) {
	char * line = readline(fp);
	free(line);
}
	
int count_lines(FILE * fp) {
	rewind(fp);
	
	int c = fgetc(fp);

	int i = 0;
		
	while(c != EOF) {
		if(c == '\n') i++;
		c = fgetc(fp);
	}
	
	rewind(fp);
	
	return i;
}

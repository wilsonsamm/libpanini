
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../panini.h"
#include "readfile.h"

#define INFILE "data.txt"

#define NONE  0
#define HAPPY 1
#define SAD   2
#define DONE  3
#define DOTS  4
#define PROGOUT stdout

char * stage;
char * learnlang = 0;
char * currentline = 0;

int progress = 1;
int totallines = 1;

void count_lines(FILE * fp) {
	rewind(fp);
	int c = fgetc(fp);
	while(c != EOF) {
		if(c == '\n') totallines++;
		c = fgetc(fp);
	}
	rewind(fp);
	return;
}

void progpc(int smile, char * stage) {
	if(stage) fprintf(PROGOUT, " %s \t", stage);
	fprintf(PROGOUT, "%3d%%", (progress * 100/totallines * 100)/100);
//	printf(", line %d,\t", progress);
	if(smile == DONE)  fprintf(PROGOUT, " All done!\n");
	if(smile == NONE)  fprintf(PROGOUT, "          \r");
	if(smile == HAPPY) fprintf(PROGOUT, " :-)      \r");
	if(smile == SAD)   fprintf(PROGOUT, " :-(      \r");
	if(smile == DOTS)  fprintf(PROGOUT, " ...      \r");
	fflush(PROGOUT);
}
void clear_progpc() {
	fprintf(PROGOUT, "                                                     \r");
}

int nextline(FILE * fp) {
	
	if(currentline) {
		free(currentline);
		currentline = 0;
	}
	
	int alloc = 10;
	int offs = 0;
	int c = '\0';
	char * line = malloc(alloc);
	line[0] = '\0';
	
	while(c != '\n') {
		c = fgetc(fp);
		if(c == EOF) {
			free(line);
			currentline = 0;
			return 1;
		}
		line[offs++] = c;
		if(offs == alloc) {
			alloc += 5;
			line = realloc(line, alloc);
		}
		line[offs] = '\0';
	}
	
	currentline = line;
	progress++;
	if(strlen(line)) return 1;
	return 0;
}

char * getfield(int n) {
	
	if(!currentline) return 0;
	if(!strlen(currentline)) return 0;
	
	int alloc = 10;
	int offs = 0;
	int loffs = 0;
	int c = '\0';
	char * line = malloc(alloc);
	line[0] = '\0';
	
	n--;
	while(currentline[offs] && n) {
		if(currentline[offs++] == ':') n--;
	}
	while(currentline[offs] == ' ') offs++;
	
	while(currentline[offs] && c != '\n' && c != ':') {
		c = currentline[offs++];
		if(c == ':') break;
		if(c == '\n') break;
		if(c == '\r') break;
		if(c == '\t') break;
		line[loffs++] = c;
		if(loffs == alloc) {
			alloc += 5;
			line = realloc(line, alloc);
		}
		line[loffs] = '\0';
	}
	return line;
}

int eof() {
//	printf("progress: %d\ttotallines: %d\n", progress, totallines);
	
	if(progress == totallines) return 1;
	return 0;
}

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pinyin.h"
#include "../../list/list.h"

int linecount;

void countlines(FILE * fp) {
	rewind(fp);
	int c;
	c = fgetc(fp);
	while(c != EOF) {
		if(c == '\n') linecount++;
	c = fgetc(fp);
	}
	rewind(fp);
}

char * utf8(char * hexcodepoint) {

	int cp = (int)strtol(hexcodepoint + 2, NULL, 16);
	
	char utf8string[8];
	int offset = 0;
	if (cp < 128) {
		utf8string[offset++] = cp;
	} else if (cp < 2048) {
	    utf8string[offset++] = cp >> 6;
	    utf8string[offset++] = cp & 63;
	}
	else if (cp < 65536) {
	    utf8string[offset++] = 224 | (cp >> 12);
	    utf8string[offset++] = 128 | ((cp >> 6) & 63);
	    utf8string[offset++] = 128 | (cp & 63);
	} else if (cp < 1114112) {
	    utf8string[offset++] = 240 | (cp >> 18);
	    utf8string[offset++] = 128 | ((cp >> 12) & 63);
	    utf8string[offset++] = 128 | ((cp >> 6) & 63);
		utf8string[offset++] = 128 | (cp & 63);
	}
	utf8string[offset] = 0;
	return strdup(utf8string);
	
}

void def_simp(FILE * out, char * codepoint) {
	fprintf(out, "(df %s (language (orthography simplified)) ", codepoint);
	fprintf(out, "(lit %s))\n", utf8(codepoint));
}

void def_trad(FILE * out, char * codepoint) {
	fprintf(out, "(df %s (language (orthography traditional)) ", codepoint);
	fprintf(out, "(lit %s))\n", utf8(codepoint));
}

void def_pinyin(FILE * out, char * codepoint, list * pinyin) {
	char * tonenumber = convert(list_get_token(pinyin, 2), PINYIN_DIA, PINYIN_NUM);
	if(!tonenumber) return;
	
	fprintf(out, "(df %s (language (orthography pinyin) -tonenumbers) ", codepoint);
	fprintf(out, "(flags %s) ", tonenumber);
	fprintf(out, "(lit %s))\n", list_get_token(pinyin, 2));

	fprintf(out, "(df %s (language (orthography pinyin) tonenumbers) ", codepoint);
	fprintf(out, "(flags %s) ", tonenumber);
	fprintf(out, "(lit %s))\n", tonenumber);
	free(tonenumber);
}

void def_mandarinsegments(FILE * out, char * codepoint, list * pinyin) {
	char * tonenumber = convert(list_get_token(pinyin, 2), PINYIN_DIA, PINYIN_NUM);
	if(!tonenumber) return;
	
	fprintf(out, "(segment (underlying %s-%s) ", codepoint, tonenumber);
	fprintf(out, "(call %s %s) ", codepoint, tonenumber);
	fprintf(out, "(language (language mandarin))) \n");
	free(tonenumber);
}

void definitions(list * codepoint) {
	
	FILE * outfile = fopen("UnihanOutput", "w");
	if(!outfile) {
		fprintf(stderr,"Could not open UnihanOutput.\nStop.\n");
		return;
	}
	
	if(!codepoint) return;
	
	int i;
	for(i = 1; i < codepoint->length; i++) {
		list * cp = list_get_list(codepoint, i);
		if(!codepoint) continue;

		char * cpname = list_get_token(cp, 1);
		list * simplified = list_find_list(cp, "kSimplifiedVariant");
		list * traditional = list_find_list(cp, "kTraditionalVariant");
		list * pinyin = list_find_list(cp, "kMandarin");

		if(!simplified)  def_simp(outfile, cpname);
		if(!traditional) def_trad(outfile, cpname);
		if(pinyin) def_pinyin(outfile, cpname, pinyin);
		if(pinyin) def_mandarinsegments(outfile, cpname, pinyin);
		
	}
}

int readfile(list * cp, char * fn) {
	FILE * infile = fopen(fn, "r");
	if(!infile) {
		fprintf(stderr,"Could not open %s.\nStop.\n", fn);
		return(99);
	}
	linecount = 0;
	countlines(infile);

	char codepoint[1024];
	char key[1024];
	char value[1024];
	int c = fgetc(infile);
	int offset;
	int count = 0;
	int linec = 0;

linestart:
	linec++;
	codepoint[0] = '\0';
	key[0] = '\0';
	value[0] = '\0';
	
	if(c == EOF) return(0);
	
	/* skip all comments */
	if(c == '#') {
		while(c != '\n' && c != '\r') c = fgetc(infile);
		c = fgetc(infile);
		if(c == EOF) return(0);
		goto linestart;
	}

	offset = 0;
	/* Read the codepoint in */
	while(c != '\t') {
		if(c == EOF) return(0);
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
	while(c != '\n' && c != '\r') {
		value[offset++] = c;
		c = fgetc(infile);
	}
	value[offset] = '\0';
	if(strlen(codepoint) && strlen(key) && strlen(value) && !strstr(codepoint, "#")) {
		//printf("%s\t%s\n",codepoint, key);
		list * c1 = list_find_list(cp, codepoint);
		if(!c1) {
			c1 = list_append_list(cp);
			list_append_token(c1, codepoint);
		}
		list * c2 = list_find_list(c1, key);
		if(!c2) {
			c2 = list_append_list(c1);
			list_append_token(c2, key);
		}
		list_append_token(c2, value);
	}

	count++;
	if(count > 100) {
		printf("%s %d%%\r", fn, (int)(linec*100)/linecount);
		fflush(stdout);
		count = 0;
	}
	
	/* End of line */
	c = fgetc(infile);
	goto linestart;
}

int main(int argc, char * argv[]) {
	int i = 1;
	list * cp = list_new();
	while(argv[i]) {
		readfile(cp, argv[i]);
		printf("%s all done.\n", argv[i++]);
	}
	definitions(cp);
	return 0;
}

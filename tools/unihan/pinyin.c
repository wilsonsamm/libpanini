#include <stdio.h>
#include <string.h>
#include "pinyin.h"
#include "unihan_lookup.h"

/* This function will convert pinyin with tonal diacritics to pinyin with tone
 * numbers. */
int pinyin_conv(char * src, char * dst, char * test, char * conv) {
	if(strncmp(src, test, strlen(test))) {
//		fprintf(stderr, "; %s did not match %s\n", src, test);
		return 0;
	} else {
//		fprintf(stderr, "; %s did match %s\n", src, test);
		strcat(dst, conv);
		return strlen(test);
	}
}

char * convert(char * pinyin, int from, int to) {
	char py[10];
	int offs = 0;
	py[0] = '\0';
	
	int i = 0;
	for(i = 0; i < INITIALS; i++) {
		if(!initials[i][from]) break;
		offs = pinyin_conv(pinyin, py, initials[i][from], initials[i][to]);
		if(offs) break;
	}

	for(i = 0; i < CODAS; i++) {
		if(!codas[i][from]) break;
		int r = pinyin_conv(pinyin + offs, py, codas[i][from], codas[i][to]);
		if(r && strlen(pinyin + offs + r)) break;
		if(r) return strdup(py);
	}
	
	return 0;
}

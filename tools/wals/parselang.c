#include "../../list/list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* number of bytes to allocate to column heading. */
#define COLHEAD 1024

int main(int argc, char * argv[]) {
	
	FILE * lang = fopen(argv[1], "r");
	if(!lang) exit(1);

	int wc = 1;
	int c = 1;
	int l = 1;
	int ch;

	ch = fgetc(lang);

	/* Search through the column headings for the column we want. The result
	 * will be stored in wc. */
	while(ch != '\n') {
		char field[COLHEAD];
		int offs = 0;

		/* Read in the entire field. */
		while(ch != '\n' && ch != ',' && ch != '\n') {
			if(ch == '\"') {
				ch = fgetc(lang);
				while(ch != '\"') {
					field[offs++] = ch;
					field[offs] = '\0';
					ch = fgetc(lang);
				}
			}
			field[offs++] = ch;
			field[offs] = '\0';
			ch = fgetc(lang);
			if(ch == EOF) exit(2);
			if(offs > COLHEAD - 2) exit(2);
		}
		c++;
		
		/* If the field is the one we wanted, then stop. */
		if(!strncmp(field, argv[2], strlen(argv[2]))) {
			wc = c;
			break;
		}
		ch = fgetc(lang);
	}
	ch = fgetc(lang); // work-around for the bug that means the last column couldn't ever be queried.
		  // (what was happening: ch would equal '\n', and the while loop down there wouldn't fire.)

	/* Search the first field of every subsequent line for the language code
	 * we're after. */
	while(ch != '\n') {
		char langcode[COLHEAD];
		int offs = 0;

		/* Read in the entire first field of the line. */
		while(ch != ',') {
			if(offs < COLHEAD) {
				langcode[offs++] = ch;
				langcode[offs] = '\0';
			}
			ch = fgetc(lang);
			if(ch == EOF) exit(2);
		}
		l++;
		if(offs > COLHEAD - 2) break;
		
		/* If this line is not the one we wanted, then go past it. */
		if(strcmp(langcode, argv[3])) {
			while(ch != '\n') ch = fgetc(lang);
		} else {
			break;
		}
		ch = fgetc(lang);
	}
	
	/* Skip so many columns that the cursor will be pointing at just the right
	 * string. */
	ch = fgetc(lang);
	wc = wc - 3;
	while(wc) {
		if(ch == '\"') {
			ch = fgetc(lang);
			while(ch != '\"') ch = fgetc(lang);
		}
		while(ch != ',') {
			ch = fgetc(lang);
			if(ch == EOF) exit(2);
		}
		ch = fgetc(lang);
		wc--;
	}
	
	/* Then print out the rest of the field. */
	if(ch == '\"') {
		ch = fgetc(lang);
		while(ch != '\"') {
			putc(ch, stdout);
			ch = fgetc(lang);
		}
	} else 
	while(ch != ',' && ch != '\n') {
		putc(ch, stdout);
		ch = fgetc(lang);
	}
	putc('\n', stdout);

	fclose(lang);

	return 0;
}

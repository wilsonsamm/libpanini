#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

list * list_new() {
	list * l = malloc(sizeof(list));
	l->allocated = 0;
	l->length = 0;
	l->types = 0;
	l->data = 0;
	return l;
}

void list_free(list * l) {
	if(!l) return;
	int i = 0;
	while(i < l->length) {
		if(!l->types) break;
		
		if(l->types[i] == TOKEN)
			free(l->data[i]);
		else if(l->types[i] == LIST)
			list_free(l->data[i]);
		i++;
	}
	free(l->types);
	free(l->data);
	free(l);
	return;
}

void list_rename(list * l, char * c) {
	/* The first element in the list, if it's a token, serves to name the list in some cases. This function, given a name and a 
	 * C-string, gives the list a new name. */
	
	if(l->types[0] != TOKEN) return;
	
	free(l->data[0]);
	l->data[0] = strdup(c);
	
	return;
}

int list_extend(list * l) {

	/* This first case deals with when there has been nothing allocated yet */
	if(!l->allocated) {
		l->types = malloc(sizeof(int) * EXTENDBY);
		if(!l->types) return 1;
		l->data = malloc(sizeof(void *) * EXTENDBY);
		if(!l->data) {
			free(l->types);
			return 1;
		}
		l->allocated = EXTENDBY;
	/* This second case deals with if we need to realloc. */
	} else {
		int newa	 = l->allocated + EXTENDBY;
		l->types	 = realloc(l->types, sizeof(int)    * newa);
		l->data		 = realloc(l->data,  sizeof(void *) * newa);
		l->allocated = newa;
	}

	l->types[l->length + 0] = UNDEF;
	l->types[l->length + 1] = UNDEF;

	return 0;
}

int list_append_token(list * l, char * token) {
	if(l->allocated == l->length) if(list_extend(l)) return 1;

	l->types[l->length] = TOKEN;
	l->data[l->length]  = strdup(token);
	l->length++;
	
	return 0;
}

list * list_append_list(list * l) {
	if(l->allocated == l->length) if(list_extend(l)) return 0;
	
	l->data[l->length]  = list_new();
	if(!l->data[l->length]) return 0;
	l->types[l->length] = LIST;	

	l->length++;
	return l->data[l->length - 1];
}

int list_prettyprinter(list * l) {
	int i = 0;
	printf("(");
		
	while(i < l->length) {
		if(!l->types) break;
		if(l->types[i] == TOKEN) printf(i == l->length - 1 ? "%s" : "%s ", (char *)l->data[i]);
		if(l->types[i] == LIST) list_prettyprinter(l->data[i]);
		i++;
	}

	printf(") ");
	return 0;
}

int list_fprettyprinter(FILE * fp, list * l) {
	int i = 0;
	fprintf(fp, "(");
		
	while(i < l->length) {
		if(!l->types) break;
		if(l->types[i] == TOKEN) fprintf(fp, i == l->length - 1 ? "%s" : "%s ", (char *)l->data[i]);
		if(l->types[i] == LIST) list_fprettyprinter(fp, l->data[i]);
		i++;
	}

	fprintf(fp, ") ");
	return 0;
}

int list_uglyprinter(list*l) {
	int i = 0;
	printf("( [length = %d; alloc'd = %d]\n", l->length, l->allocated);
		
	while(i < l->length) {
		if(!l->types) break;
		
		if(l->types[i] == TOKEN) {
			printf("TOKEN\t0x%x\t%s\n", (int)l->data[i], (char *)l->data[i]);
		}
		else if(l->types[i] == LIST) {
			printf("LIST\t0x%x\n", (int)l->data[i]);
			list_uglyprinter(l->data[i]);
		}
		i++;
	}

	printf(")\n");
	return 0;
}
	
/* This function gets the nth token out of the list. If the nth element is not a
 * token you'll get the NULL pointer. */
char * list_get_token(list * l, int n) {
	n--;

#ifdef BOUNDSCHECK
	if(n < 0) return 0;
	if(l->length < n) return 0;
	
	if(!l->types) return 0;
	if(!l->data) return 0;
#endif
	
	if(l->types[n] != TOKEN) return 0;
	return l->data[n];
}

/* This function gets the nth list out of the list. If the nth element is not a
 * list you'll get the NULL pointer. */
list * list_get_list(list * l, int n) {

	n--;

#ifdef BOUNDSCHECK
	/* Bounds checking */
	if(n < 0) return 0;
	if(l->length < n) return 0;

	/* Is the list not empty? */
	if(!l->types) return 0;
	if(!l->data) return 0;
#endif

	if(l->types[n] != LIST) return 0;
	return l->data[n];
}

/* A wrapper to __list_tokeniser that gives it a C string */
int list_tokenise_chars(list * l, char * string) {
	list * toks = list_new();
	__list_tokeniser(toks, string);
	__list_treebuilder(toks, l, 1);
	list_free(toks);
	return 0;
}
	

/* A wrapper to __list_tokeniser that gives it the contents of a file. */
int list_tokenise_file(list * l, FILE *fp) {
	int alloc = 1024;
	int readin = 0;
	char * f = malloc(alloc);
	f[0] = '\0';
	list * toks = list_new();
	
	int ch = 0;

	int paren = 0;

	rewind(fp);
	
	for( ; ; ) {
		ch = fgetc(fp);
		if(ch == EOF) {
			f[readin] = '\0';
			break;
		}
		
		f[readin] = (char) ch;
		readin++;
		if(readin == alloc) {
			alloc += 1024;
			f = realloc(f, alloc);
		}

		if((char)ch == '(') paren++;
		if((char)ch == ')') paren--;

		if(paren < 0) {
			free(f);
			list_free(toks);
			return 1;
		}
	}

	if(paren) {
		free(f);
		list_free(toks);
		return 1;
	}
	
	__list_tokeniser(toks, f);
	free(f);

	__list_treebuilder(toks, l, 1);
	list_free(toks);

	return 0;
}

/* CSV file tokeniser
 * (csv files can be read by mainstream spreadsheets: both OpenOffice and Microsoft Excel handle CSV files well enough). */
list * list_tokenise_csv(FILE *fp) {
	int alloc = 16;
	int readin = 0;
	char * f = malloc(alloc);
	f[0] = '\0';
	
	int ch = 0;

	rewind(fp);
	
	list * file = list_new();
	list * line = list_append_list(file);
	list * cell = list_append_list(line);

	for( ; ; ) {
		
		/* Are we getting near the end of allocated memory? then we need to allocate some more. */
		if(readin > alloc - 3) {
			alloc += alloc;
			f = realloc(f, alloc);
		}
		
		/* Read a character in from the file. */
		ch = fgetc(fp);
		if(ch != EOF) f[readin++] = (char) ch;
		
		/* Have we reached the end of a string? If so, append the string to the cell */
		if(ch == EOF || ch == '\n' || ch == '\t' || ch == ' '  || ch == ',') {
			f[readin - 1] = '\0';
			list_append_token(cell, f);
			readin = 0;
			f[readin] = '\0';
		}
		
		/* Have we reached the end of the cell? (these are comma-seperated) */
		if(ch == ',' ) {
			cell = list_append_list(line);
		}
		
		/* Have we reached the end of the line? (these are newline-seperated) */
		if(ch == '\n' ) {
			line = list_append_list(file);
			cell = list_append_list(line);
		}
		
		/* Have we reached the end of the file? (if so, stop reading the file) */
		if(ch == EOF) break;
		
	}
	
	free(f);
	return file;
}

list * list_find_list(list * l, char * lname) {
	int i;
	list * p;
	char * t;
	for(i = 1; i <= l->length; i++) {
		p = list_get_list(l, i);
		if(!p) continue;

		t = list_get_token(p, 1);
		if(!t) continue;

		if(!strcmp(t, lname)) return p;
	}
	return 0;
}

int list_append_copy(list * dst, list * src) {
	int i;
	char * t;
	list * l;
	for(i = 1; i <= src->length; i++) {
		if((t = list_get_token(src, i)))
			list_append_token(dst, t);
		if((l = list_get_list(src, i)))
			list_append_copy(list_append_list(dst), l);
	}
	return 0;
}

/* Returns 1 if the list contains the token, zero otherwise */
int list_contains(list * l, char * t) {
	int i;
	for(i = 1; i < l->length; i++) {
		if(l->types[i] != TOKEN) continue;

		if(!strcmp(l->data[i], t)) return 1;
	}
	return 0;
}

/* Returns 1 if the list contains the token prepended with '-', zero otherwise */
int list_contains_neg(list * l, char * t) {
	int i;
	for(i = 1; i < l->length; i++) {
		if(l->types[i] != TOKEN) continue;
		
		if(((char *)l->data[i])[1] != '-') continue;
		
		if(!strcmp(l->data[i]+1, t)) return 1;
	}
	return 0;
}

void list_remove(list * l, char * c) {
	int i;
	char * t = 0;
	list * k;

	/* Find the occurrences of a list or token by the name specified by char * c,
	 * and delete them */
	for(i = 1; i <= l->length; i++) {
		t = list_get_token(l, i);
		if(t) {
			if(!strcmp(t, c)) {
				list_drop(l, i);
//				continue;
			}
		}

		k = list_get_list(l, i);
		if(k) {
			t = list_get_token(k, 1);
			if(!strcmp(t, c)){
				list_drop(l, i);
				i--;
			}
		} 
	}
	
}


int list_drop(list * l, int offset) {
	/* If the list is empty, don't do anything at all */
	if(l->length == 0) return 1;
	
	/* Another special case is if the list has only one element */
//	if(l->length == 1) {
//		l->length = 0;
//		l->data[0] = 0;
//		l->types[0] = UNDEF;
//		return 1;
//	}
	
	int i = offset - 1;
	if(l->types[i] == TOKEN) free(l->data[i]);
	if(l->types[i] == LIST)  list_free(l->data[i]);
	
	l->length--;
	
	for(i = offset - 1; i < /* sic */ l->length; i++) {

		l->data[i] = l->data[i+1];
		l->types[i] = l->types[i+1];
	}

	l->data[l->length]  = 0;
	l->types[l->length] = UNDEF;
	
	return 0;
}

#ifndef __LIST_H
#define __LIST_H

#include <stdio.h>

#define TOKEN 1
#define LIST 2
#define UNDEF 3

typedef struct __list {
	int allocated;	/* Should be how many entries are allocated for */
	int length;		/* Should be how many entries in the array */
	int * types;
	void ** data;
} list;
int list_drop(list * l, int offset);
list * list_new();
void list_free(list * l);
int list_append_token(list * l, char * token);
list * list_append_list(list * l);
char * list_get_token(list * l, int n);
list * list_get_list(list * l, int n);
list * list_find_list(list * l, char * lname);
int list_tokenise_chars(list * l, char * toks);
int list_tokenise_file(list * l, FILE * fp);
list * list_tokenise_csv(FILE * fp);
int list_prettyprinter(list * l);
int list_fprettyprinter(FILE * fp, list * l);
int list_append_copy(list * dst, list * src);
list * __list_tokeniser(list * l, const char * toks);
unsigned int __list_treebuilder(list * toks, list * target, unsigned int offset);
int list_contains(list * l, char * c);
void list_remove(list * l, char * c);
void list_rename(list * l, char * c);

#endif

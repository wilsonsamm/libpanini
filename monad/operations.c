
#include "monad.h"
#include "../list/list.h"
#include <string.h>

int set_trace(monad * m, int * n) {
	m->trace = *n;
	return 0;
}

int set_intext(monad * m, char * n) {
	/* Round brackets need to be switched for square ones, for syntactic
	 * reasons. */
	int i;
	int len = strlen(n);
	for(i=0; i<len; i++) {
		if(n[i] == '(') n[i] = '[';
		if(n[i] == ')') n[i] = ']';
	}
	m->intext = n;
	m->index = 0;
	return 0;
}

int set_stack(monad * m, char * stack) {
	if(m->stack) list_free(m->stack);
	
	if(m->command) list_free(m->command);

	m->stack = list_new();
	list_tokenise_chars(m->stack, stack);

	return 0;
}

int set_seme(monad * m, char * seme) {
	if(!m->namespace) m->namespace = list_new();
	list_remove(m->namespace, "seme");
	list * s = list_append_list(m->namespace);
	list_append_token(s, "seme");
	
	list_tokenise_chars(s, seme);

	return 0;
}

int print_out(monad * m, FILE * fp) {
	if(m->outtext) fprintf(fp,"%s\n", m->outtext);
	return 0;
}

int print_ns(monad * m, void * nothing) {
	printf("\nMonad %d:\nBRAKE: %d\nCONFIDENCE: %d\n", m->id, m->brake, m->confidence);
	list_prettyprinter(m->namespace);
	printf("\n");
	return 0;
}

int print_seme(monad * m, void * nothing) {
	list * s = list_find_list(m->namespace, "seme");
	list_prettyprinter(s);
	return 0;
}

int remove_ns(monad * m, char * n) {
	if(m->namespace) list_remove(m->namespace, n);
	return 0;
}
int kill_least_confident(monad * m, void * nothing) {
	int klc(monad * m, void * n) {
		if(m->confidence <= *(int*)n) m->alive = 0;
		else *(int*)n = m->confidence;
		return 0;
	}
	int c = m->confidence;
	monad_map(m->child, klc, &c, -1);
	if(c > m->confidence) m->alive = 0;
	return 0;
}

int kill_identical_outtexts(monad * m, void * nothing) {
	int kio(monad * m, void * n) {
		if(!m->outtext) return 0;
		if(!strcmp((char*)n, m->outtext)) m->alive = 0;
		return 0;
	}
	
	if(!m->outtext) return 0;
	
	monad_map(m->child, kio, m->outtext, -1);
	return 0;
}

int kill_braked_monads(monad * m, int * thr) {
	
	if(!m->brake > *thr) m->alive = 0;
	
	return 0;
}

int kill_not_done(monad * m, void * nothing) {
	if(!m->stack) return 0;
	
	if(m->stack->length) m->alive = 0;
	
	return 0;
}

// Search the monads for the highest confidence
// The integer winds up getting put where c points
int max_confidence(monad * m, int * c) {
	if(m->confidence > *c) *c = m->confidence;
	return 0;
}

// Kill any monads less confident than the integer pointed to by c
int kill_less_confident(monad * m, int * c) { 
	if(m->confidence < *c) m->alive = 0;
	return 0;
}
	
// Put a (record) in each monad's namespace
int append_record_ns(monad * m, void * nothing) {
	if(m->namespace == 0) m->namespace = list_new();
	if(!list_find_list(m->namespace, "record")) {
		list * r = list_append_list(m->namespace);
		list_append_token(r, "record");
	}
	return 0;
}
	
// Set the edit distance
int set_edit(monad * m, int * edit) {
	char str[15];
	sprintf(str, "%d", *(int*)edit);
	
	if(m->namespace == 0) m->namespace = list_new();
	list_remove(m->namespace, "edit");
	
	list * editns = list_append_list(m->namespace);
	list_append_token(editns, "edit");
	list_append_token(editns, str);
	
	return 0;
}

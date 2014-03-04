/* This file contains functions that are visible to the outside world. */

#include "monad.h"
#include "../list/list.h"
#include <stdlib.h>
#include <string.h>

monad * monad_new() {
	monad * m = malloc(sizeof(monad));
	
	m->rules = 0;
	m->alive = 1;
	m->namespace = 0;
	m->scopestack = 0;
	m->stack = 0;
	m->child = 0;
	m->command = 0;
	m->readahead = 0;
	m->id = 1;
	m->capital = 1;
	m->outtext = strdup("");
	m->index = 0;
	m->trace = 0;
	m->debug = 0;
	m->confidence = 0;
	m->debug = 0;
	m->brake = 0;
	m->howtobind = 0;
	m->intext = 0;
	m->outtext = 0;
	m->parent_id = 0;

	return m;
}

monad * monad_duplicate(monad * m) {
	if(!m) return 0;
	while(!m->alive) {
		m = m->child;
		if(!m) return 0;
	}
	
	monad * n = monad_new();
	n->rules = 0;
	n->alive = m->alive;
	n->namespace = list_new();
	if(m->namespace) list_append_copy(n->namespace, m->namespace);
	n->scopestack = list_new();
	if(m->scopestack) list_append_copy(n->scopestack, m->scopestack);
	n->stack = list_new();
	if(m->stack) list_append_copy(n->stack, m->stack);
	n->command = list_new();
	if(m->command) list_append_copy(n->command, m->command);
	
	n->id = m->id;
	if(m->readahead) n->readahead = strdup(m->readahead);
	
	n->capital = m->id;
	if(m->outtext) n->outtext = strdup(m->outtext);
	n->index = m->index;
	n->trace = m->trace;
	n->debug = m->debug;
	n->confidence = m->confidence;
	n->brake = m->brake;
	n->child = 0;
	n->howtobind = m->howtobind;
	return n;
}

void monad_set_trace(monad * m, int trace) {
	while(m) {
		m->trace = trace;
		m = m->child;
	}
}

void monad_set_intext(monad * m, char * c) {
	m->intext = c;
	m->index = 0;
}


void __monad_free(monad * m) {
	if(m->outtext)	free(m->outtext);
	if(m->namespace)	list_free(m->namespace);
	if(m->scopestack)	list_free(m->scopestack);
	if(m->stack)	list_free(m->stack);
	if(m->readahead)	free(m->readahead);
	if(m->child)	__monad_free(m->child);

	free(m);
	return;
}

void monad_free(monad * m) {
	if(!m) return;

	if(m->rules) list_free(m->rules);
	__monad_free(m);

	return;
}

list * __monad_rule_loader(char * lang) {
	/* Prepare the filename */
	char * fn = malloc(1024);
	strcpy(fn, FN_PATH);
	strcat(fn, lang);

//	printf("Loading rules from %s\n", fn);

	/* Open the file and tokenise it, and then close it */
	FILE * fp = fopen(fn, "r");
	free(fn);
	list * rules = list_new();

	list_tokenise_file(rules, fp);

	fclose(fp);

	return rules;
}

int monad_rules(monad * m, char * lang) {

	list * rules = __monad_rule_loader(lang);

	/* Free existing rules from the monads */
	if(m->rules) list_free(m->rules);

	/* Set the RULES register on all the monads to point to the rules we just
	 * loaded */
	while(m) {
		m->rules = rules;
		m = m->child;
	}

	return 0;
}


int monad_popcom(monad * m) {
	if(!m->stack) {
		fprintf(stderr, "Monad %d has no stack!\n", m->id);
	}
	
	if(m->stack->length == 0) {
		if(m->debug) {
			printf("Monad %d had nothing left to do so I've put ", m->id);
			printf("a NULL pointer into the INSTRUCTION register ");
		}
		return 0;
	}

	m->command = list_new();

	list * com = list_get_list(m->stack, 1);
	if(!com) {
		fprintf(stderr, "What is this? Something is wrong with the stack in ");
		fprintf(stderr, "Monad %d. \nHere it is: ", m->id);
		list_prettyprinter(m->stack);
		m->alive = 0;
	}

	list_append_copy(m->command, com);

	list_drop(m->stack, 1);

	return 0;
}

void print_debugging_info(monad * m) {
	printf("\n\nStepping Monad %d. ", m->id);
	printf("This monad is %s.\n", m->alive ? "alive":"dead");
	if(m->stack) {
		printf("Stack:");
		list_prettyprinter(m->stack);
	} else {
		printf("This monad has no stack.");
	}
	printf("\nNamespace: ");
	if(m->namespace) list_prettyprinter(m->namespace);
	printf("\nHowtobind flags: ");
	if(m->howtobind & STRICT) printf("STRICT ");
	if(m->howtobind & WRITE) printf("WRITE ");
	if(m->howtobind & CREATE) printf("CREATE ");
	if(m->howtobind & BLOCK) printf("BLOCK ");
	printf("\n");
	printf("Intext: (%d) %s\n", m->index, m->intext);
	printf("Outtext: %s\n", m->outtext);
	printf("Readahead: %s\n", m->readahead);
	printf("Scopestack:");
	if(m->scopestack) list_prettyprinter(m->scopestack);
	printf("\nParent: %d\n", m->parent_id);
}

/* This function calls the chosen function on the monads that:
 *  - are still alive. 
 *  - the BRAKE register must not be higher than the given threshold 
 * And returns:
 *  0 if none if the monads are still alive after calling the function 
 *  1 if there are live monads left.
 *
 * If the function returns a non-zero value, then it will be called on the monad again.
 */
int monad_map(monad * m, int(*fn)(monad * m, void * argp), void * arg, int thr) {

	int retval = 0;
	
	/* Run! */
	while(m) {
		if(m->trace == m->id) m->debug = 1;
		if(!m->alive) m->debug = 0;
		if(m->debug) print_debugging_info(m);

		/* Make sure the monad we are about to process really is alive. */
		if(!m->alive) {
			m = m->child;
			continue;
		}
		
		/* And its BRAKE register must be below the threshold (if there is a threshold at all */
		if(thr != -1 && m->brake > thr) {
			if(m->debug && m->stack) {
				fprintf(stderr, "BRAKE = %d.  THRESHOLD = %d.\n", m->brake, thr);
				fprintf(stderr, "This monad has been skipped since it's BRAKE register is too high.\n");
			}
			
			m = m->child;
			continue;
		}
		
		/* Call the function then! */
		if(!fn(m, arg))	{
			if(m->alive) retval = 1;
			m = m->child;
			continue;
		}
	}
	return retval;
}

int set_trace(monad * m, int * n) {
	m->trace = *n;
	return 0;
}

int set_intext(monad * m, char * n) {
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

int kill_not_done(monad * m, int * thr) {
	if(!m->stack) return 0;
	
	if(m->stack->length) m->alive = 0;
	
	return 0;
}

int unlink_the_dead(monad * m, void * nothing) {
	if(m->alive) return 0;
	
	if(m->command) list_free(m->command);
	m->command = m->child->command;
	
	if(m->stack) list_free(m->stack);
	m->stack = m->child->stack;
	
	if(m->namespace) list_free(m->namespace);
	m->namespace = m->child->namespace;
		
	if(m->scopestack) list_free(m->scopestack);
	m->scopestack = m->child->scopestack;
				
	if(m->outtext) free(m->outtext);
	m->outtext = m->child->outtext;
	if(m->readahead) free(m->readahead);
	m->readahead = m->child->readahead;
	
	m->intext = m->child->intext;
	m->brake = m->child->brake;
	m->howtobind = m->child->howtobind;
	m->alive = m->child->alive;
	m->debug = m->child->debug;
	m->trace = m->child->trace;
	m->confidence = m->child->confidence;
	m->capital = m->child->capital;
	m->parent_id = m->child->parent_id;
	m->id = m->child->id;
	m->index = m->child->index;
	
	monad * tmp = m->child->child;
	free(m->child);
	m->child = tmp;
	
	return 1;
}

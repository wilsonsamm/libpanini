#ifndef __MONAD_H
#define __MONAD_H
#include "../list/list.h"

/* The path where we'll find our files */
#define FN_PATH	"/usr/tranny/languages/"

/* Here is a list of "how to bind" flags */
#define STRICT	1 /* Variable must already be bound in GENERATE (but not in PARSE) */
#define CREATE	2 /* Scopes may be created */
#define WRITE	4 /* Variables may be created */
#define BLOCK	8 /* Delete the variables, don't create them */

/* The monad. I have put all the pointers at the beginning of the monad
 * so as to keep alignment problems from swelling up the amount of memory
 * this takes. */
typedef struct _monad {
	list * command;		/* Contains the command currently executing */
	list * stack;		/* Contains the list of commands to execute 
						   next */
	list * rules;		/* Rules for parsing */
	
	list * namespace;	/* Variables can be bound in here. */
	list * scopestack;	/* This keeps track of which scope we're 
						   currently in */
	
	char * outtext;		/* This is where the translation will be put */
	char * intext;		/* This is where the original text should be */
	int index;			/* How far through the intext have we already 
						   scanned ? */
	int brake;			/* Brake -- If this number increases beyond some
						 * boundary, then the monad is paused. */
	int howtobind;		/* Part of the command */

	struct _monad * child;		/* The linked list of spawned monads */

	int alive;			/* Is the monad alive? */
	int debug;
	int trace;
	int confidence;
	int capital;
	int parent_id;
	int id;
} monad;

int partialstrcmp(char * scanme, char * word);

monad * monad_new();
void monad_free(monad * m);
int monad_grules(monad * m, char * lang);
int monad_prules(monad * m, char * lang);
void monad_set_program(monad * m, int prog);
int monad_run(monad * m);
void monad_set_intext(monad * m, char * c);
void monad_set_trace(monad * m, int i);
int monad_popcom(monad * m);

int bind(list * namespace, list * vars);
char * eval(list * namespace, char * varname);

monad * monad_spawn(monad * m, list * rules, list * flags);

int tranny_parse(monad * m, void * nothing);
int tranny_generate(monad * m, void * nothing);
int tranny_gowild(monad * m, void * nothing);
int tranny_learn(monad * m, FILE * output);

void monad_parse_constituent(monad * m);
void monad_parse_nop();
void monad_parse_req(monad * m);
void monad_parse_into(monad * m);
void monad_parse_return(monad * m);
void monad_parse_debug(monad * m);
void monad_parse_confidence(monad * m);
void monad_parse_brake(monad * m);
void monad_parse_unbrake(monad * m);
void monad_parse_fork(monad * m);
void monad_parse_fuzzy(monad * m);
void monad_parse_block(monad * m);
void monad_parse_adjunct(monad * m);
void monad_parse_strict(monad * m);
void monad_parse_lit(monad * m);
void monad_parse_space(monad * m);
void monad_parse_fullstop(monad * m);
void monad_parse_forgive(monad * m);
void monad_parse_open(monad * m);
void monad_generate_lit(monad * m);
void monad_generate_space(monad * m);
void monad_generate_fullstop(monad * m);
void monad_generate_strict(monad * m);
void monad_generate_forgive(monad * m);

char * evaluate(monad * m, list * var);
void speculate(monad * m, char * namespace, char * varname);
void bind_vars(monad * m);

int monad_init(monad * m, char * command);
int monad_test(monad * m);
int monad_test_all_dead(monad * m);
monad * monad_get_child(monad * m);
int monad_get_program(monad * m);
int monad_get_confidence(monad * m);
monad * monad_outtoin(monad * m);
void monad_keep_unique(monad * m);
char * monad_get_outtext(monad * m);

list * get_namespace(monad * m, char * nsname);

#endif

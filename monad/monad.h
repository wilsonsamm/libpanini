#ifndef __MONAD_H
#define __MONAD_H
#include "../list/list.h"

#define THRESHOLD 20 /* How many times a monad will brake before it will buy the farm */

/* The path where we'll find our files */
#define FN_PATH	"/usr/panini/languages/"

/* Here is a list of switches. These alter the programs' behaviour as they run.
 */
#define SPELLCHK	1 // Activates "spellchecker mode". 
#define FORGIVE		2 // Forgives common grammatical mistakes.


/* The monad. I have put all the pointers at the beginning of the monad
 * so as to keep alignment problems from swelling up the amount of memory
 * this takes. */
typedef struct _monad {
	list * command;		/* Contains the command currently executing */
	list * stack;		/* Contains the list of commands to execute next */
	list * rules;		/* Rules for parsing */
	int ralloc;
	
	list * namespace;	/* Variables can be bound in here. */
	list * scopestack;	/* This keeps track of which scope we're currently in */
	
	char * outtext;		/* This is where the translation will be put */
	char * intext;		/* This is where the original text should be */
	int index;			/* How far through the intext have we already scanned ? */
	int brake;			/* Brake -- If this number increases beyond some boundary, then the monad is paused. */

	struct _monad * child;		/* The linked list of spawned monads */

	int alive;			/* Is the monad alive? */
	int debug;
	int trace;
	int confidence;
	int switches;
	struct _monad * adjunct;
	int parent_id;
	int id;
} monad;

int partialstrcmp(char * scanme, char * word);

monad * monad_new();
void monad_free(monad * m);
int monad_rules(monad * m, char * lang);
void monad_join(monad * to, monad * addition);
int monad_map(monad * m, int(*fn)(monad * m, void * argp), void * arg, int threshold);
int monad_popcom(monad * m);

monad * monad_duplicate(monad * m);

int bind(list * namespace, list * vars);
char * eval(list * namespace, char * varname);

monad * monad_spawn(monad * m, list * rules, list * flags);

int tranny_parse(monad * m, void * nothing);
int tranny_generate(monad * m, void * nothing);
int tranny_gowild(monad * m, void * nothing);
int tranny_learn(monad * m, void * nothing);

void monad_parse_constituent(monad * m, int adjunct);
void monad_parse_nop();
void monad_parse_req(monad * m);
void monad_parse_into(monad * m, int head);
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
void monad_parse_check(monad * m);
void monad_parse_readahead(monad * m);
void monad_parse_sandhiblock(monad * m);

void monad_generate_lit(monad * m);
void monad_generate_space(monad * m);
void monad_generate_fullstop(monad * m);
void monad_generate_strict(monad * m);
void monad_generate_forgive(monad * m);
void monad_generate_readahead(monad * m);
void monad_parse_segments(monad * m);

char * strlist(list * l, char * takeme);

char * evaluate(monad * m, list * var);
void speculate(monad * m, char * namespace, char * varname);
void bind_vars(monad * m);

int monad_init(monad * m, char * command);
int monad_test_all_dead(monad * m);
monad * monad_get_child(monad * m);

monad * monad_outtoin(monad * m);

monad * monad_copy_one(monad * m);

int kill_identical_outtexts(monad * m, void * nothing);

char * monad_get_outtext(monad * m);
int set_stack(monad * m, char * stack);
int set_intext(monad * m, char * n);
int set_edit(monad * m, int * edit);
int remove_ns(monad * m, char * n);
int print_ns(monad * m, void * nothing);
int print_out(monad * m, FILE * fp);
int unlink_the_dead(monad * m, void * nothing);
int set_trace(monad * m, int * i);
int kill_not_done(monad * m, void * nothing);
int kill_least_confident(monad * m, void * nothing); // Kills monads except the most confident ones.
int append_record_ns(monad * m, void * nothing);
int max_confidence(monad * m, int * c);
int kill_less_confident(monad * m, int * c);

#endif

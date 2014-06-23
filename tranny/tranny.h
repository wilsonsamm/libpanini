/* This file contains some important declarations for the various interpreters of the Tranny language. 
 * (tranny is the name of the language that the source code found in the languages/ directory is written in. That source code
 * is interpreted by the routines in the tranny/ directory. */

/* This function tests for and executes a small set of little instructions. */
int tranny_misc(monad * m, char * command);

/* This function tests for and executes those instructions that are responsible for binding variables. */
int tranny_binders(monad * m, int gen);
int check_vars(list * namespace, list * vars);

/* These functions tests for and executes those instructions that are responsible for scanning the input/output. */
int tranny_intext(monad * m, char * command);
int tranny_outtext(monad * m, char * command);

/* These functions tests for and executes those instructions that spawn other monads. */
int tranny_exec(monad * m, char * command, int(reduce)(monad * m, list * l));

/* These functions have to do with memory. */
int tranny_memory(monad * m, char * command);


/* */
int parse_reduce(monad * m, list * l);
int generate_reduce(monad * m, list * l);

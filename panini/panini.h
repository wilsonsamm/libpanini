
/*
 * These switches define how the program will apply to the given inputs. For
 * example, to interpret a sentence, to yield its meaning, OUTTEXT should be 
 * off, and INTEXT should be on, and CR_SEME should be on.
 * When the program runs, the panini_execute function is called, and the switch
 * parameter holds the switches that are turned on OR'ed together into a single
 * machine word.
 */
 

#define OUTTEXT  1 // Should we append the text to the OUTTEXT?
#define CR_SEME  2 // Can we create variables in the seme namespace?
#define INTEXT   4 // Should we take the string from the INTEXT?
#define L_OPEN   8 // Can we use (open...) to learn?
#define L_TAG	16 // Does (tag ...) do anything or does it get ignored?
#define P_OPEN  32 // Can we use (open...) to learn?
#define TAG     64 // Can we use (tag...) to attest?
#define RECORD 128 // Should we record each segment as we process it?


void learn_tag(monad * m);
void guess_segments(monad * m);
void monad_learn_open(monad * m);

int reduce(monad * m, list * l, int * switches);
list * get_namespace(monad * m, char * nsname, int create);
void append_to_outtext(monad * m, char * segment);
void scan_intext(monad * m, char * segment);
void panini_call(monad * m, int adjunct, int * switches);
void panini_fork(monad * m, int * switches);
void panini_segments(monad * m, int generate);
void panini_fuzzy(monad * m);
void panini_fork(monad * m, int * switches);

#include "../monad/monad.h"

int panini_parse(monad *m, char * commands, char * intext, int editdistance, int record, int threshold) {
	
	int retval;
	
	/* If we want to record the segments, then do so */
	if(record) monad_map(m, append_record_ns, (void*)0, threshold);
	
	/* If we have to set some edit distance, then do so */
	if(editdistance) monad_map(m, (int(*)(monad * m, void * argp))set_edit, &editdistance, threshold);
	
	/* First, set the stack to contain the right instructions */
	monad_map(m, (int(*)(monad * m, void * argp))set_stack, commands, threshold);
	
	/* Next, set the INTEXT up */
	monad_map(m, (int(*)(monad * m, void * argp))set_intext, intext, threshold);
	
	/* Then, parse the instructions and the INTEXT */
	retval = monad_map(m, tranny_parse, (void*)0, threshold);
	
	/* Then, kill any monad that didn't finish the program. */
	monad_map(m, kill_not_done, (void *)0, -1);
	
	/* and free any monads that are not still alive
	 * (this forgets their state so that the memory becomes free. */
	monad_map(m, unlink_the_dead, m, -1);
	
	return retval;
}

int panini_learn(monad * m, char * commands, FILE * out, char * intext, int threshold) {
	
	int retval;
	
	/* First, set the stack to contain the right instructions */
	monad_map(m, (int(*)(monad * m, void * argp))set_stack, commands, threshold);
	
	/* Next, set the INTEXT up */
	monad_map(m, (int(*)(monad * m, void * argp))set_intext, intext, threshold);
	
	/* Then, learn the language given the intext */
	retval = monad_map(m, tranny_learn, (void*)0, threshold);
	
	/* Then, kill any monad that didn't finish the program. */
	monad_map(m, kill_not_done, (void *)0, -1);
	
	/* and free any monads that are not still alive
	 * (this forgets their state so that the memory becomes free. */
	monad_map(m, unlink_the_dead, m, -1);
	
	/* Then print out what we learned. */
	monad_map(m, (int(*)(monad * m, void * argp))print_out, out, threshold);
	
	return retval;
}
	
	

int panini_generate(monad *m, char * commands, int record, int threshold) {
	
	int retval;
	
	/* First, set the stack to contain the right instructions */
	monad_map(m, (int(*)(monad * m, void * argp))set_stack, commands, threshold);
	
	/* Then, parse the instructions, generating OUTTEXT */
	retval = monad_map(m, tranny_generate, (void*)0, threshold);
	
	/* Then, kill any monad that didn't finish the program
     * and free any dead monads so that they don't occupy memory. */
	monad_map(m, kill_not_done, (void *)0, -1);
	monad_map(m, unlink_the_dead, m, -1);
	
	return retval;
}

int panini_keep_confident(monad * m) {
	
	/* Find the highest confidence of all the monads */
	int confidence;
	monad_map(m, (int(*)(monad * m, void * argp))max_confidence, &confidence, -1);
	
	/* The kill all monads that are less confident */
	int retval = monad_map(m,  (int(*)(monad * m, void * argp))kill_less_confident, &confidence, -1);
	
	monad_map(m, unlink_the_dead, (void*)0, -1);
	return retval;
}
	

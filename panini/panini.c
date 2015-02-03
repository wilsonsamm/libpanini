#include "../monad/monad.h"
#include "panini.h"
#include "tranny.h"

#include <string.h> 

int panini_despatch(monad * m, int * switches) {
		
	/* Pop the next command */
	monad_popcom(m);
	
	/* If there were no more commands to execute, then we're done and the monad
	 * survives. */
	if(!m->command) return 0;
		
	/* Don't bother continuing if the monad is dead */
	if(!m->alive) return 0;
	
	/* Debugging info */
	if(m->debug) {
		printf("Monad %d is executing: ", m->id);
		if(m->command) list_prettyprinter(m->command);
		printf("\n");
	}
	
	char * command = list_get_token(m->command, 1);
	
	/* Is the current command a set of miscellaneous ones that work the same in all interpreters? */
	if(tranny_misc(m, command)) return 1;
	
	/* Is the current command one that either:
	 *   - matches something against the input, or
	 *   - appends something to the output,
	 * depending on whether or not we're generating? */
	if((*switches & INTEXT)  && tranny_intext (m, command)) return 1;
	if((*switches & OUTTEXT) && tranny_outtext(m, command)) return 1;
	
	/* Is the command one of those that spawns other monads? */
	if(tranny_exec(m, command, switches)) return 1;
	
	/* Is the command one of the ones deals with memory? */
	if(tranny_memory(m, command)) return 1;
	
	/* Is the command one of the ones that binds variables? */
	if( (*switches & CR_SEME) && (tranny_binders(m, 1))) return 1;
	if(!(*switches & CR_SEME) && (tranny_binders(m, 0))) return 1;
	
	/* Is the command one of the ones that generates rules? */
	if(tranny_phrase(m, command)) return 1;
	
	if(!strcmp(command, "guess-segments")) {
		guess_segments(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	if(!strcmp(command, "into")) {
		monad_parse_into(m, (*switches & OUTTEXT), (*switches & INTEXT));
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "return")) {
		monad_parse_return(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "open")) {
		if(*switches & L_OPEN) monad_learn_open(m);
		if(*switches & P_OPEN) monad_parse_open(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "tag")) {
		learn_tag(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s.\n", command);

	m->alive = 0;
	return 0;
}

int panini_parse(monad *m, char * commands, char * intext, int editdistance, int record, int threshold) {
	
	int retval;
	int switches = CR_SEME | INTEXT;
	
	/* If we want to record the segments, then do so */
	if(record) monad_map(m, append_record_ns, (void*)0, threshold);
	
	/* If we have to set some edit distance, then do so */
	if(editdistance) monad_map(m, (int(*)(monad * m, void * argp))set_edit, &editdistance, threshold);
	
	/* First, set the stack to contain the right instructions */
	monad_map(m, (int(*)(monad * m, void * argp))set_stack, commands, threshold);
	
	/* Next, set the INTEXT up */
	monad_map(m, (int(*)(monad * m, void * argp))set_intext, intext, threshold);
	
	/* Then, parse the instructions and the INTEXT */
	retval = monad_map(m, (int(*)(monad * m, void * argp))panini_despatch, &switches, threshold);
	
	/* Then, kill any monad that didn't finish the program. */
	monad_map(m, kill_not_done, (void *)0, -1);
	
	/* and free any monads that are not still alive
	 * (this forgets their state so that the memory becomes free. */
	monad_unlink_dead(m, 0);
		
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
	
	/* Then, kill any monad that didn't finish the program, */
	monad_map(m, kill_not_done, (void *)0, -1);

	/* and any that have a higher brake value than any other, */
	monad_kill_braked(m);
	
	/* and any that have not finished processing the intext */
	monad_kill_unfinished_intext(m);
	
	/* and then free any monads that are not still alive
	 * (this forgets their state so that the memory becomes free. */
	monad_unlink_dead(m, 0);

	/* Then print out what we learned. */
	monad_map(m, (int(*)(monad * m, void * argp))print_out, out, threshold);
	
	return retval;
}
	
int panini_generate(monad *m, char * commands, int record, int threshold) {
	
	int retval;
	int switches = OUTTEXT;
	
	/* First, set the stack to contain the right instructions */
	monad_map(m, (int(*)(monad * m, void * argp))set_stack, commands, threshold);
	
	/* Then, parse the instructions, generating OUTTEXT */
	retval = monad_map(m, (int(*)(monad * m, void * argp))panini_despatch, &switches, threshold);
	
	/* Then, kill any monad that didn't finish the program
     * and free any dead monads so that they don't occupy memory. */
	monad_map(m, kill_not_done, (void *)0, -1);
	monad_unlink_dead(m, 0);
	
	return retval;
}

int panini_keep_confident(monad * m) {
	
	/* Find the highest confidence of all the monads */
	int confidence;
	monad_map(m, (int(*)(monad * m, void * argp))max_confidence, &confidence, -1);
	
	/* The kill all monads that are less confident */
	int retval = monad_map(m,  (int(*)(monad * m, void * argp))kill_less_confident, &confidence, -1);
	
	monad_unlink_dead(m, 0);
	return retval;
}
	

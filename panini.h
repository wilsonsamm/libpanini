/* THIS HEADER FILE SHOULD NEVER BE USED BY ANY FILE THAT COMPILES INTO LIBPANINI.A. At install time it is copied to /usr/includes,
 * so that other programs can link against libtranny.a. The point of this is black-box abstraction: programs that link against 
 * libtranny shouldn't ever poke around in the monad struct, for example (this approach makes the thing more future proof too).
 */

#ifndef __TRANNY_H
#define __TRANNY_H

/* If your program uses libtranny, you still won't be able to poke 
 * around in the monad structure! */
typedef void monad;

/* This function creates a new monad. */
monad * monad_new();

/* This function frees a monad so that it's no longer occupying the computer's memory. */
monad * monad_free();

/* This function calls the chosen function on the monads that:
 *  - are still alive. 
 *  - the BRAKE register must not be higher than the given threshold 
 * And returns:
 *  0 if none if the monads are still alive after calling the function 
 *  1 if there are alive functions.
 */
int monad_map(monad * m, int(*fn)(monad * m, void * argp), void * arg, int threshold);

/* This function loads a particular language. */
void monad_rules(monad * m, void * t);
monad * monad_duplicate(monad * m); 
monad * monad_duplicate_all(monad * m); 

/* These following functions can be used with monad_map to set various parts of the state of all monads. */
int set_trace(monad * m, void * n); // TRACE says "Set the debug flag on this monad."
int set_intext(monad * m, void * t); // INTEXT says "Translate this."
int set_stack(monad * m, void * s); // STACK says "Do this."
int set_seme(monad * m, void * d); // SEME contains some meaning or other.
int remove_ns(monad * m, void * n); // Takes something out of the namespace. 
int print_out(monad * m, void * nothing); // Prints the OUTTEXT out.
int print_ns(monad * m, void * nothing); // Prints the NAMESPACE out.
int print_seme(monad * m, void * nothing); // Prints (seme ...) out.
int kill_identical_outtexts(monad * m, void * nothing); // Kills monads that have the same OUTTEXT.
int kill_least_confident(monad * m, void * nothing); // Kills monads except the most confident ones.
int kill_braked_monads(monad * m, void * thr); // Kills monads where the BRAKE is higher than *(int*)thr.
int kill_not_done(monad * m, void * nothing); // Kills monads that have not finished their program.
int unlink_the_dead(monad * m, void * nothing); // Removes dead monads from the linked list.
int set_switches(monad * m, void * switches); // Sets switches
int clear_switches(monad * m, void * switches); // Clears switches

/* These function interpret a Tranny program */
int tranny_parse(monad * m, void * nothing);      // This one parses the incoming text and produces the meaning.
int tranny_generate(monad * m, void * nothing);   // This one generates a new text, given the meaning stored in the monads.
int tranny_learn(monad * m, void * filepointer);  // This takes the incoming text, and the meaning, and then learns, producing
                                                  // (unattested) rules that may be added to the tranny program.
int tranny_attest(monad * m, void * nothing);     // This, if it ever runs an unattested rule and succeeds, then that rule will be
                                                  // attested.

#endif

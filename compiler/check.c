#define LANGPATH "./languages/"
#include "../list/list.h"
#include "compiler.h"

#include <stdlib.h>
#include <string.h>

int crawl(list * output, int (fn)(list * def, char * rname)) {
	int i;
	
	for(i = 1; i<= output->length; i++) {
		int j;
		list * definitions = list_get_list(output, i);
		char * defname = list_get_token(definitions, 1);
		for(j = 2; j <= definitions->length; j++) {
			list * def = list_get_list(definitions, j);
			if(def)
				if(fn(def, defname))
					return 1;
		}
	}
	return 0;
}

int check_syntax1(list * input) {
	int i;
	list * command;
	char * c;
	for(i = 1; i <= input->length; i++) {
		command = list_get_list(input, i);

		if(!command) {
			c = list_get_token(input, i);

			printf("What is the token %s doing here?\n", c);
			return 1;
		}

		c = list_get_token(command, 1);

		if(!c) {
			fprintf(stderr, "\tOne of the commands does not start ");
			fprintf(stderr, "with a token.\n");
			return 1;
		}
		
		if(!strcmp(c, "include"))			continue;
		if(!strcmp(c, "for"))				continue;
		if(!strcmp(c, "df"))				continue;
		if(!strcmp(c, "segment"))			continue;
		if(!strcmp(c, "sandhi-initial"))	continue;
		if(!strcmp(c, "sandhi-final"))		continue;
		
		fprintf(stderr, "\tI do not know any such command as ");
		fprintf(stderr, "\"%s\".\n", c);
		return 1;
		
	}
	return 0;
}

int check_debug(list * output) {
	int test(list * def, char * name) {
		if(list_find_list(def, "debug")) {
			fprintf(stderr, "\tAt least one definition of ");
			fprintf(stderr, "%s has a (debug) statement ", name);
			fprintf(stderr, "in it.\n");
		}
		return 0;
	}
	
	crawl(output, test);
	return 0;
}


int check_wronginstruction(list * output) {
	int test(list * def, char * name) {
		int i;
		for(i = 1; i <= def->length; i++) {
			list * instr; // the instruction
			instr = list_get_list(def, i);
			if(!instr) continue;
			char * iname; // name of the instruction
			iname = list_get_token(instr, 1);
			if(!strcmp(iname, "into")) iname = list_get_token(instr, 3);
			if(!strcmp(iname, "lit")) continue;
			if(!strcmp(iname, "constituent")) continue;
			if(!strcmp(iname, "flags")) continue;
			if(!strcmp(iname, "sandhi")) continue;
			if(!strcmp(iname, "fullstop")) continue;
			if(!strcmp(iname, "debug")) continue;
			if(!strcmp(iname, "rection")) continue;
			if(!strcmp(iname, "seme")) continue;
			if(!strcmp(iname, "space")) continue;
			if(!strcmp(iname, "confidence")) continue;
			if(!strcmp(iname, "brake")) continue;
			if(!strcmp(iname, "forgive")) continue;
			if(!strcmp(iname, "fuzzy")) continue;
			if(!strcmp(iname, "fork")) continue;
			if(!strcmp(iname, "adjunct")) continue;
			if(!strcmp(iname, "strict")) continue;
			if(!strcmp(iname, "open")) continue;
			if(!strcmp(iname, "theta")) continue;
			if(!strcmp(iname, "attest")) continue;
			if(!strcmp(iname, "block")) continue;
			if(!strcmp(iname, "language")) continue;
			if(!strcmp(iname, "check")) continue;
			if(!strcmp(iname, "read-ahead")) continue;
			if(!strcmp(iname, "clues")) continue;
			if(!strcmp(iname, "nop")) continue;
			if(!strcmp(iname, "segments")) continue;
			
			fprintf(stderr, "\tWhat is this?\n");
			fprintf(stderr, "\tA definition of %s has an ", name);
			fprintf(stderr, "instruction %s.\n", iname);
			return 1;
		}
		return 0;
	}
	return crawl(output, test);
}

int check_deprecated(list * output) {
	int test(list * def, char * name) {
		int i;
		for(i = 1; i <= def->length; i++) {
			list * instr; // the instruction
			instr = list_get_list(def, i);
			if(!instr) continue;
			char * iname; // name of the instruction
			iname = list_get_token(instr, 1);
			if(!strcmp(iname, "into")) iname = list_get_token(instr, 3);
			if(strcmp(iname, "seme")) continue;
			
			if(list_find_list(instr, "rel")) {
				fprintf(stderr, "\tIn at least one definition of %s:\n", name);
				fprintf(stderr, "\tThe deprecated variable \"rel\" is being bound in the seme namespace\n");
				return 1;
			}
		}
		return 0;
	}
	return crawl(output, test);
}

/* This function checks rules for instructions that don't do anything, and removes them. */
int check_removenops(list * output) {
	int test(list * def, char * name) {
		int i;
		for(i = 1; i <= def->length; i++) {
			list * instr; // the instruction
			instr = list_get_list(def, i);
			if(!instr) continue;
			char * iname; // name of the instruction
			iname = list_get_token(instr, 1);
			
			/* Instructions that bind variables, but don't specify 
			 * anything to bind, can be removed. 
			 * Empty flag lists can be removed. */
			if(instr->length == 1 && \
			  (!strcmp(iname, "rection") || \
			   !strcmp(iname, "sandhi")  || \
			   !strcmp(iname, "seme")    || \
			   !strcmp(iname, "concord") || \
			   !strcmp(iname, "flags"))) {
					list_drop(def, i);
					i--;
					continue;
			}
			
			/* nops can be removed. */
			if(!strcmp(iname, "nop")) {
				list_drop(def, i);
				i--;
				continue;
			}
		}
		return 0;
	}
	crawl(output, test);
	return 0;
}

/* This function checks rules for instructions that don't do anything, and removes them. */
int check_space(list * output) {
	int space(list * def, char * name) {
		if(!strcmp(name, "space")) return 0;
		int i;
		for(i = 1; i <= def->length; i++) {
			list * instr; // the instruction
			instr = list_get_list(def, i);
			if(!instr) continue;
			char * iname; // name of the instruction
			iname = list_get_token(instr, 1);
			if(!iname) continue;
			if(strcmp(iname, "space")) continue;
			
			list_drop(instr, 1);
			list_append_token(instr, "constituent");
			list_append_token(instr, "space");
			
		}
		return 0;
	}
	if(list_find_list(output, "space")) crawl(output, space);
	return 0;
}

/* Since natural languages can go on recursively ad infinitum, and therefore won't fit in a finite state machine, this function adds
 * the (brake) instruction to recursive rules like:
 *  (df nounphrase
 * 		(into possessor parse nounphrase possessive)
 * 		(parse noun possessive)
 * 		(flags possessive))
 * The (brake) instruction will appear at the front of such rules. And what it does is to increment the BRAKE register. Once that 
 * goes beyond some limit, the monad is just paused so that it won't find an answer. 
 * 
 * (this is at least what appears to happen. It's simpler to add (brake) to EVERY rule and then delete it from rules that don't need
 * them.) 
 * 
 * (this is also not infallible. This does for example not pick up on circular interdependencies like :
 * the bell on the cat, that sat on the mat, which lay on the floor, which... 
 * but will pick up on things like:
 *   the bean in the bag in the tin in the box in another bag in the kitchen...)
 */
int check_recursion(list * output) {
	int brake(list * def, char * name) {
		int i;
		list * brake = 0;
		for(i = 1; i <= def->length; i++) {
			list * instr; // the instruction
			instr = list_get_list(def, i);
			if(!instr) continue;
			char * iname; // name of the instruction
			iname = list_get_token(instr, 1);
			
			if(!strcmp(iname, "brake")) {
				brake = instr;
				continue;
			}
			
			if(!strcmp(iname, "into")) {
				if(!strcmp("constituent", list_get_token(instr, 3))) {
					if(!strcmp(name, list_get_token(instr, 4))) {
						return 0;
					}
				}
			}
			
			if(!strcmp(iname, "constituent")) {
				if(!strcmp(name, list_get_token(instr, 2))) {
					return 0;
				}
			}
			
		}
		if(brake) {
			while(brake->length) list_drop(brake, 1);
			list_append_token(brake, "nop");
		}
		return 0;
	}
	
	int unbrake(list * def, char * name) {
		if(list_find_list(def, "brake")) {
			list * unbrake = list_append_list(def);
			list_append_token(unbrake, "unbrake");
		}
		return 0;
	}
	crawl(output, brake);
	crawl(output, unbrake);
	return 0;
}

int check_main_exists(list * output) {
	if(!list_find_list(output, "main")) {
		fprintf(stderr, "\tNo definitions for main.\n");
		return 1;
	}
	return 0;
}


/* It is theoretically faster to bind variables as early as possible (monads are likely to die when binding variables; if they die
 * earlier, less time is spent on them), so this routine moves variables bound in later instructions upward, like so. If you define 
 * something like this:
 * (df noun
 * 		(lit Haus)
 * 		(flags count neuter)
 * 		(seme (head house)))
 * when this rule is defined, an extra (seme) is inserted at the beginning like so:
 * (df noun
 * 		(seme)
 * 		(lit Haus)
 * 		(flags count neuter)
 * 		(seme (head house)))
 * On its own, it doesn't do anything. But this function moves anything from the second seme to the first:
 * (df noun
 * 		(seme (head house))
 * 		(lit Haus)
 * 		(flags count neuter)
 * 		(seme))
 * Later, the second seme will get removed. This whole process has effectively moved the (seme) instruction up to the top. A
 * similar thing happens with (rection). Before I did this, it consistently took between 27 and 28 seconds to translate "I see a
 * whale" into Swahili. Now it takes around 21 seconds. I was hoping for a little more dramatic improvement, but that's life.
 */
int check_early_binding(list * output) {
	int move_binding(list * def, char * name) {
		int i;
		list * seme = 0;
		list * rection = 0;
		
		for(i = 1; i <= def->length; i++) {
			
			list * instr; // the instruction
			instr = list_get_list(def, i);
			if(!instr) continue;
			
			char * iname; // name of the instruction
			iname = list_get_token(instr, 1);
			if(!iname) continue;
			
			if(!strcmp(iname, "seme")) {
				if(!seme) {
					seme = instr;
				} else {
					list * tmp = list_new();
					list_append_copy(tmp, instr);
					list_drop(tmp, 1);
					list_append_copy(seme, tmp);
					list_free(tmp);
					
					while(instr->length > 1) list_drop(instr, 2);
				}
			}
			
			if(!strcmp(iname, "rection")) {
				if(!rection) {
					rection = instr;
				} else {
					list * tmp = list_new();
					list_append_copy(tmp, instr);
					list_drop(tmp, 1);
					list_append_copy(rection, tmp);
					list_free(tmp);
					
					while(instr->length > 1) list_drop(instr, 2);
				}
			}
		}
		return 0;
	}
	crawl(output, move_binding);
	return 0;
}

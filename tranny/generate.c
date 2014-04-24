#include "../monad/monad.h"
#include "../list/list.h"
#include <stdlib.h>
#include <string.h>

void append_string(monad * m, char * c) {
	if(!m->outtext) {
		m->outtext = strdup(c);
		return;
	}
	
	char * newtext = malloc(strlen(m->outtext) + strlen(c) + 1);
	
	strcpy(newtext, m->outtext);
	strcpy(newtext + strlen(newtext), c);
	free(m->outtext);
	m->outtext = newtext;
}

void cedechar(monad * m, char c) {
	if(!m->outtext) return;
	
	int len = strlen(m->outtext) - 1;
	if(m->outtext[len] == c) m->outtext[len] = '\0';
	return;
}

void monad_generate_fullstop(monad * m) {
	list_remove(m->namespace, "sandhi");
	cedechar(m, ' ');
	append_string(m, ".");
}

void monad_generate_lit(monad * m) {
	if(m->namespace) list_remove(m->namespace, "sandhi");
	char * morpheme = list_get_token(m->command, 2);
	append_string(m, morpheme);
}

void monad_generate_space(monad * m) {
	if(!m->outtext) return;
	if(!strlen(m->outtext)) return;
	cedechar(m, ' ');
	append_string(m, " ");
}

void monad_generate_forgive(monad * m) {
	list * todo = list_new();
	list_drop(m->command, 1);
	list_append_copy(todo, m->command);

	/* Update the stack */
	list * newstack = list_new();
	list_append_copy(list_append_list(newstack), todo);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	
	return;
}

void monad_generate_strict(monad * m) {
	/* Set the STRICT flag */
	m->howtobind |= STRICT;
	
	/* The command that's to be done strictly */
	list_drop(m->command, 1);
	list * todo = list_new();
	list_append_copy(todo, m->command);

	/* Update the stack */
	list * newstack = list_new();
	list_append_copy(list_append_list(newstack), todo);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	return;
}

void monad_generate_open(monad * m) {
	/* Does it say (rel open) (open ...) ? */
	list * ns = get_namespace(m, "seme");
	
	if(!ns) {
		m->alive = 0;
		return;
	}
	
	list * rel = list_find_list(ns, "head");
	if(!rel) {
		if(m->debug) {
			printf("There is no binding for head in the seme namespace.\n");
		}
		m->alive = 0;
		return;
	}
	char * r = list_get_token(rel, 2);
	if(strcmp(r, "open")) {
		if(m->debug) {
			printf("It does not say (rel open) in the seme namespace.\n");
		}
		m->alive = 0;
		return;
	}
	list * open = list_find_list(ns, "open");
	if(!open) {
		if(m->debug) {
			printf("It says (head open) in the seme namespace,");
			printf("but there is no (open ...) binding.\n");
			printf("This is an ontological error and there is ");
			printf("nothing I can do about it.\n");
		}
		m->alive = 0;
		return;
	}
	char * word = list_get_token(open, 2);
	
	/* Output the word, surrounded by whitespace. */	
	monad_generate_space(m);
	
	list_drop(m->command, 1);
	list_drop(m->command, 1);
	list_append_token(m->command, "lit");
	list_append_token(m->command, word);
	monad_generate_lit(m);
	monad_generate_space(m);
	m->brake++;
	return;
}

int tranny_generate(monad * m, void * nothing) {
	if(!m->alive) return 0;
	
	/* Pop the next command */
	monad_popcom(m);
	if(!m->command) return 0;
	
	if(m->debug) {
		printf("Monad %d is executing: ", m->id);
		if(m->command) list_prettyprinter(m->command);
		printf("\n");
	}

	char * command = list_get_token(m->command, 1);
	
	if(!strcmp(command, "call")) {
		monad_parse_constituent(m, 0);
		list_free(m->command);
		m->command = 0;
		m->alive = 0;
		return 0;
	}
	if(!strcmp(command, "strict")) {
		monad_generate_strict(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}	
	if(!strcmp(command, "block")) {
		monad_parse_block(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "constituent")) {
		monad_parse_constituent(m, 0);
		list_free(m->command);
		m->command = 0;
		m->alive = 0;
		return 0;
	}
	if(!strcmp(command, "capital")) {
		m->capital = 1;
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "flags")) {
		monad_parse_nop();
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "theta")) {
		
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "rection")) {
		
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "sandhi")) {
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "concord")) {
		m->howtobind |= WRITE | CREATE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "lit")) {
		monad_generate_lit(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "space")) {
		monad_generate_space(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "seme")) {
		//m->howtobind &= ~(CREATE | WRITE);
		m->howtobind |= WRITE;
		//m->howtobind &= ~(CREATE);
		//m->howtobind = 0;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "confidence")) {
		monad_parse_confidence(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "into")) {
		monad_parse_into(m, 1);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "fullstop")) {
		monad_generate_fullstop(m);
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
	if(!strcmp(command, "debug")) {
		monad_parse_debug(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "brake")) {
		monad_parse_brake(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "unbrake")) {
		monad_parse_unbrake(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "forgive")) {
		monad_generate_forgive(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "fork")) {
		monad_parse_fork(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "adjunct")) {
		monad_parse_constituent(m, 1);
		list_free(m->command);
		m->command = 0;
		//m->brake++;
		return 1;
	}
	if(!strcmp(command, "fuzzy")) {
		monad_parse_fuzzy(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "open")) {
		monad_generate_open(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "nop")) {
		monad_parse_nop(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "attest")) {
		//monad_parse_nop(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "language")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "check")) {
		monad_parse_check(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "read-ahead")) {
		//monad_parse_nop(m);
		m->alive = 0;
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "clues")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program GENERATE.\n", command);

	m->alive = 0;
	return 0;
}

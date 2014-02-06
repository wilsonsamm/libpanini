#include "../monad/monad.h"
#include <string.h>
#include <stdlib.h>


/* This function is like strcmp except it only looks as far as (char*)y goes */
int partialstrcmp(char * x, char * y) {
	int i;

	for(i = 0; y[i]; i++) {
		if(!x[i]) return y[i];
		if(x[i] != y[i]) return x[i] - y[i];
	}
	return 0;
}

void monad_parse_constituent(monad * m) {
	/* We'll get the list of rules that need to be parsed */
	list * patterns = 0;
	char * rulename = list_get_token(m->command, 2);
	patterns = list_find_list(m->rules, rulename);
	
	list * flags = list_new();
	int i;
	for(i = 3; i <= m->command->length; i++) {
		char * f = list_get_token(m->command, i);
		if(f) list_append_token(flags, f);
	}
	
	/* If there is such a list, spawn the children. */
	if(patterns) {
		if(!monad_spawn(m, patterns, flags)) {
			if(m->debug) {
				printf("Couldn't find any definitions for ");
				printf("%s with the ", list_get_token(m->command, 2));
				printf("right flags.\n");
			}
		}

	/* Otherwise, maybe print some debugging info. */
	} else {
		if(m->debug) {
			printf("Couldn't find any definitions for ");
			printf("%s at all.\n", list_get_token(m->command, 2));
		}
	}
	list_free(flags);
	return;
}

void monad_parse_nop() {
	return;
}

void monad_parse_lit(monad * m) {
	if(m->namespace) list_remove(m->namespace, "sandhi");
	char * morpheme = list_get_token(m->command, 2);
	if(partialstrcmp(m->intext + m->index, morpheme)){
		if(m->debug) {
			printf("%s does not match %s.\n", morpheme, m->intext + m->index);
		}
		m->alive = 0;
		return;
	}
	
	/* A morpheme has two boundaries, and they never have the same 
	 * sandhi properties. */
	if(m->namespace) list_remove(m->namespace, "sandhi");
	
	m->index += strlen(morpheme);
	return;
}

void monad_parse_space(monad * m) {

	if(m->index == 0) return;
	if(m->intext[m->index-1] == ' ') return;
	
	if(m->debug) 
		printf("INDEX: %d \t INTEXT_LENGTH: %d\n", m->index, strlen(m->intext));
	if(m->index == strlen(m->intext)) return;
	
	if(!partialstrcmp(m->intext + m->index, ".")) {
		return;
	}
	if(partialstrcmp(m->intext + m->index, " ")){
		if(m->debug) {
			printf("%s does not match whitespace.\n", m->intext + m->index);
		}
		m->alive = 0;
		return;
	}
	m->index += strlen(" ");
	return;
}

void monad_parse_fullstop(monad * m) {
	if(m->namespace) list_remove(m->namespace, "sandhi");
	if(partialstrcmp(m->intext + m->index, ".")){
		if(m->debug) {
			printf("%s does not match a full stop.\n", m->intext + m->index);
		}
		m->alive = 0;
		return;
	}
	m->index += strlen(".");
	return;
}


void monad_parse_into(monad * m) {
	/* The name of the scope we need to enter */
	char * intowhat = list_get_token(m->command, 2);
	if(!intowhat) intowhat = evaluate(m, list_get_list(m->command, 2));
	if(!intowhat) {
		if(m->debug)
			printf("Couldn't not evaluate which scope to enter.\n");
		m->alive = 0;
		return;
	}
	
	if(m->debug) {
		printf("The scope we are going to enter is called ");
		printf("%s.\n", intowhat);
	}
	if(!m->scopestack) m->scopestack = list_new();
	list_append_token(m->scopestack, intowhat);
	
	/* The command to execute in that scope */
	list * toexecute = list_new();
	list_append_copy(toexecute, m->command);
	list_drop(toexecute, 1);
	list_drop(toexecute, 1);

	/* The (return) command */
	list * retcom = list_new();
	list_append_token(retcom, "return");

	/* Update the stack */
	list * newstack = list_new();
	list_append_copy(list_append_list(newstack), toexecute);
	list_append_copy(list_append_list(newstack), retcom);
	list_append_copy(newstack, m->stack);
	list_free(m->stack);
	m->stack = newstack;
	if(m->debug) {
		printf("This will be the new stack:\n");
		list_prettyprinter(m->stack);
		printf("\n");
	}

	/* Tidy up */
	list_free(retcom);
	list_free(toexecute);
}


void monad_parse_return(monad * m) {
	list_drop(m->scopestack, m->scopestack->length);
}

void monad_parse_debug(monad * m) {
	m->debug = 1;
	return;
}

void monad_parse_confidence(monad * m) {
	m->confidence += atoi(list_get_token(m->command, 2));
	return;
}

void monad_parse_brake(monad * m) {
	m->brake++;
	if(m->brake > 20) m->alive = 0;
}

void monad_parse_unbrake(monad * m) {
	m->brake--;
}

void monad_parse_forgive(monad * m) {
	list * rules = list_new();
	list * parent = list_append_list(rules);
	list * child = list_append_list(rules);
	list * brake = list_append_list(parent);
	list * todo = list_append_list(child);
	
	list_append_token(brake, "brake");
	
	list_drop(m->command, 1);
	list_append_copy(todo, m->command);
	
	monad_spawn(m, rules, 0);
}

void monad_parse_fork(monad * m) {
	list_drop(m->command, 1);
	monad_spawn(m, m->command, 0);
	m->alive = 0;
}

void monad_parse_fuzzy(monad * m) {
	list * rules = list_new();
	list_append_list(rules);
	list * child = list_append_list(rules);
	list * confidence = list_append_list(child);
	list * todo = list_append_list(child);
	
	list_append_token(confidence, "confidence");
	list_append_token(confidence, list_get_token(m->command, 2));
	
	list_drop(m->command, 1);
	list_drop(m->command, 1);
	list_append_copy(todo, m->command);
	
	monad_spawn(m, rules, 0);
	m->alive = 0;
}

void monad_parse_strict(monad * m) {
	list * rules = list_new();
	list_append_list(rules);
	list * child = list_append_list(rules);
	list * todo = list_append_list(child);
	
	list_drop(m->command, 1);
	list_append_copy(todo, m->command);
	
	monad_spawn(m, rules, 0);
	m->alive = 0;
}

void monad_parse_block(monad * m) {
	/* Set the BLOCK flag */
	m->howtobind |= BLOCK;
	
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

void monad_parse_attest(monad * m, char * langname) {
	if(!langname) return;
	
	/* Open the attest file */
	char * fn = malloc(strlen(langname) + strlen("/usr/tranny/attest/") + 1);
	strcpy(fn, "/usr/tranny/attest/");
	strcpy(fn + strlen(fn), langname);
	FILE * at_file = fopen(fn, "a+");
	if(!at_file) {
		fprintf(stderr, "Could not open %s for writing.\n", fn);;
		free(fn);
		return;
	}
	
	free(fn);
	
	/* Construct the rule that will be written to the attest file */
	list * attest = list_new();
	list_append_token(attest, "df");
	list_drop(m->command, 1);
	list_append_copy(attest, m->command);
	list_fprettyprinter(at_file, attest);
	return;
}

void monad_parse_open(monad * m) {
	/* how long is the instring? */
	char word[1024];
	char ch;
	int i;
	
	m->brake++;
	/* Skip whitespace */
	while(m->intext[m->index] == ' ') m->index++;
	
	for(i = 0; i<1024; i++) {
		ch = m->intext[m->index];
		if(ch == ' ') break;
		word[i] = ch;
		m->index++;
	}
	word[i] = '\0'; /* null-termination */
	if(m->debug) printf("(open) found the word \"%s\".\n",word);
}	
	
int tranny_parse(monad * m, void * nothing) {
	if(!m->alive) return 0;
	
	monad_popcom(m);

	if(!m->command) return 0;

	if(m->debug) {
		printf("Monad %d is executing: ", m->id);
		if(m->command) list_prettyprinter(m->command);
		printf("\n");
	}

	char * command = list_get_token(m->command, 1);
	
	if(!strcmp(command, "strict")) {
		monad_parse_strict(m);
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
		monad_parse_constituent(m);
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
	if(!strcmp(command, "nop")) {
		monad_parse_nop();
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
	if(!strcmp(command, "concord")) {
		m->howtobind |= CREATE | WRITE;
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
	if(!strcmp(command, "lit")) {
		monad_parse_lit(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "space")) {
		monad_parse_space(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "seme")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "sandhi")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "theta")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "rection")) {
		m->howtobind |= CREATE | WRITE;
		bind_vars(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "into")) {
		monad_parse_into(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "fullstop")) {
		monad_parse_fullstop(m);
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
	if(!strcmp(command, "debug")) {
		monad_parse_debug(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "forgive")) {
		monad_parse_forgive(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "fuzzy")) {
		monad_parse_fuzzy(m);
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
		monad_parse_constituent(m);
		list_free(m->command);
		m->command = 0;
		m->brake++;
		return 1;
	}
	if(!strcmp(command, "open")) {
		monad_parse_open(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	if(!strcmp(command, "attest")) {
		monad_parse_brake(m);
		list_free(m->command);
		m->command = 0;
		return 1;
	}
	
	printf("No such command as %s in the program PARSE.\n", command);

	m->alive = 0;
	return 0;
}

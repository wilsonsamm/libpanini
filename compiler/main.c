#define LANGPATH "./languages/"
#define LEARNEDPATH "/usr/tranny/learned/"
#define ATTESTEDPATH "/usr/tranny/attested/"

#include "../list/list.h"
#include "compiler.h"

#include <stdlib.h>
#include <string.h>

/* This global function hackishly passes a parameter to include(). */
char * global_fn;

int paren_tester(FILE * fp) {
	int paren = 0;
	int ch = ' ';
	int line = 1;
	for( ; ; ) {
		if((char)ch == '(') paren++;
		if((char)ch == ')') paren--;
		if((char)ch == '\n') line++;
		if((char)ch == ';') {
			while((ch = fgetc(fp)) != '\n');
			line++;
		}
		
		if(paren < 0) {
			fprintf(stderr,"\tThere is a ')' without a matching '(' ");
			return line;
		}
		ch = fgetc(fp);
		if(ch == EOF) break;
	}
	rewind(fp);
	if(paren) {
		fprintf(stderr, "\tThere is a '(' without a matching ')' ");
		return -1;
	}
	return 0;
}

int include(list * command, list * input, list * output) {
	/* Construct the filename we want */
	char * operand = list_get_token(command, 2);
	char * filename = malloc(strlen(global_fn) + strlen("/") + strlen(LANGPATH) + strlen(operand) + 1);

	strcpy(filename, LANGPATH);
	strcpy(filename + strlen(filename), global_fn);
	strcpy(filename + strlen(filename), "/");
	strcpy(filename + strlen(filename), operand);

	FILE * fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "Couldn't open the file called %s\n", filename);
		return 1;
	}
	
	int retval;
	if((retval = paren_tester(fp))) {
		fprintf(stderr, " in line %d of %s.\n", retval, filename);
		return 1;
	}
	
	list_tokenise_file(input, fp);
	fclose(fp);

	free(filename);
	return 0;
	
}

int pass(list * input, list * output, char * token, int (*fn)(list * command, list * input, list * output)) {
	int i;
	for(i = 0; i <= input->length; i++) {
		list * command = list_get_list(input, i);
		if(!command) continue;

		char * test = list_get_token(command, 1);
		if(strcmp(test, token)) continue;

		if(fn(command, input, output)) {
			list_free(output);
			list_free(input);
			return 1;
		}
	}
	return 0;
}

void printout(list * output) {
	int i;
	for(i = 0; i <= output->length; i++) {
		if(list_get_list(output, i))
			list_prettyprinter(list_get_list(output, i));
	}
//	list_prettyprinter(output);
	return;
}

int load_file(list * input, char * directory, char * langname) {
	/* Construct the filename we want */
	char * filename = malloc(strlen(directory) + strlen(langname) + 1);

	strcpy(filename, directory);
	strcpy(filename + strlen(filename), langname);

	FILE * fp = fopen(filename, "r");
	if(!fp) {
		free(filename);
		return 0;
	}
	
	int retval = paren_tester(fp);
	if(retval) {
		fprintf(stderr, "\tParenthesis mismatch %d in %s.\n", retval, filename);
		return 1;
	}
	
	list_tokenise_file(input, fp);
	fclose(fp);

	free(filename);
	return 0;
	
}

int main(int argv, char * argc[]) {
	list * input = list_new();
	list * output = list_new();

	global_fn = argc[1];
	
	/* Neat trick to get the thing to load the file called main. */
	list_tokenise_chars(input, "(include main)");
	load_file(input, LEARNEDPATH, argc[1]);
	load_file(input, ATTESTEDPATH, argc[1]);

	/* First run all the (include) commands. They take a filename as the first
	 * argument, and loads that file, and appends it to the end of the input.
	 * In other words it's akin to #include in the C preprocessor. */
	if(pass(input, output, "include", include)) {
		return 1;
	}

	if(check_syntax1(input)) {
		fprintf(stderr, "\tThis is a fatal error.\n\tExiting.\n");
		list_free(input);
		list_free(output);
		return 1;
	}

	/* The (for ...) macro works a bit like in Bash */
	pass(input, output, "for", for_);
	
	/* Next pass is to create all the definitions, (def's are mostly dictionary definitions and such) 
	 * and then to check they're OK. */
	pass(input, output, "segment", segment);
	pass(input, output, "df", df);
	check_debug(output);
	check_wronginstruction(output);
	check_main_exists(output);
	
	/* Next pass is to put all the sandhi rules in place. */
	pass(input, output, "sandhi-initial", sandhi_initial);
	pass(input, output, "sandhi-final", sandhi_final);
	
	/* Dirty tricks */
	check_recursion(output);
	check_deprecated(output);
	check_space(output);
	
	/* Optimisations */
	check_early_binding(output);
	check_removenops(output);
	printout(output);
	
	/* Check for ontological sanity */
	check_ontology(output);
	
	/* Free stuff */
	list_free(output);
	list_free(input);
	return 0;
}
	

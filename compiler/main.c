#define LANGPATH "./languages/"


#include "../list/list.h"
#include "compiler.h"

#include <stdlib.h>
#include <string.h>

char * global_fn;
char * langname;

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

	strcpy(filename, global_fn);
	strcat(filename, "/");
	strcat(filename, operand);

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
	strcat(filename, langname);

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
	global_fn = 0;
	int i = 1;
	
	/* Neat trick to get the thing to load the file called main. */
	list_tokenise_chars(input, "(include main)");
	
	for( ; ; ) {
		if(!argc[i]) break;
		
		// This switch says which directory to look for more source code
		// in. This is used for example by (include ...)
		if(!strcmp("-d", argc[i])) {
			i++;
			global_fn = argc[i++];
			continue;
		}
		// This switch says which file in that directory to open first
		if(!strcmp("-i", argc[i])) {
			i++;
			langname = argc[i];
			load_file(input, LANGPATH, argc[i++]);
			continue;
		}
		if(!strcmp("-I", argc[i])) {
			i++;
			load_file(input, "./", argc[i++]);
			continue;
		}
		if(!strcmp("-l", argc[i])) {
			i++;
			load_file(output, "./", argc[i++]);
			continue;
		}
	}
	if(!global_fn) {
		fprintf(stderr, "You forgot to specify the directory where ");
		fprintf(stderr, "I should look for more source code.\n");
		list_free(input);
		list_free(output);
		exit(99);
	}

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
	
	/* The (tag ...) grabs a symbol from another file. Useful for cross-
	 * linguistically common features */
	pass(input, output, "tag", tagg);
	
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
	

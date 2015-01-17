#include "../../list/list.h"
#include "compiler.h"

#include <stdlib.h>
#include <string.h>

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

int include(list * command, list * input, char * dirname, int mand) {
	
	/* Construct the filename we want */
	char * operand = list_get_token(command, 2);
	char * filename = malloc(strlen(dirname) + strlen("/") + strlen(operand) + 1);

	strcpy(filename, dirname);
	strcat(filename, "/");
	strcat(filename, operand);

	FILE * fp = fopen(filename, "r");
	if(!fp && mand) {
		fprintf(stderr, "Couldn't open the file called %s\n", filename);
		free(filename);
		return 1;
	}
	if(!fp) {
		free(filename);
		return 0;
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

int include_pass(list * input, char * dirname) {
	int i;
	for(i = 1; i <= input->length; i++) {
		list * command = list_get_list(input, i);
		if(!command) continue;
		
		char * test = list_get_token(command, 1);
		if(!strcmp(test, "include")) include(command, input, dirname, 1);
		if(!strcmp(test, "load"))    include(command, input, dirname, 0);
	}
	
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

int passdf(list * input, list * output, char * token, char * outfn) {
	int i;
	for(i = 0; i <= input->length; i++) {
		list * command = list_get_list(input, i);
		if(!command) continue;

		char * test = list_get_token(command, 1);
		if(strcmp(test, token)) continue;

		if(df(command, input, output, outfn)) {
			list_free(output);
			list_free(input);
			return 1;
		}
	}
	return 0;
}

void printout(list * output, FILE * outfile) {
	int i;
	for(i = 0; i <= output->length; i++) {
		if(list_get_list(output, i)) {
			list_fprettyprinter(outfile, list_get_list(output, i));
			fprintf(outfile, "\n");
		}
	}
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

void link(list * output, list * in) {
	int i;
	for(i = 1; i <= in->length; i++) {
		list * symb = list_get_list(in, i);
		if(!symb) continue;
		
		char * symbname = list_get_token(symb, 1);
		
		/* If this symbol has not yet been defined in the output, then we can
		 * just copy the definitions in. */
		list * esym = list_find_list(output, symbname);
		if(!esym) {
			list_append_copy(list_append_list(output), symb);
		} 
		
		/* Otherwise, we need to find the symbol name in the output and then 
		 * append each definition of that symbol in */
		else {
			int j;
			for(j = 1; j <= in->length; j++) {
				list * def = list_get_list(symb, j);
				if(!def) continue;
				
				list_append_copy(list_append_list(esym), def);
			}
		}
	}
}

int linkfile(list * output, char * filename) {
	FILE * fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "\tCouldn't open the file called \"%s\"", filename);
		fprintf(stderr, "which I tried to link in.\n");
		return 1;
	}
	int retval;
	if((retval = paren_tester(fp))) {
		fprintf(stderr, " in line %d of %s.\n", retval, filename);
		return 1;
	}
	
	list * in = list_new();
	list_tokenise_file(in, fp);
	link(output, in);
	list_free(in);
	return 0;
}

int main(int argv, char * argc[]) {
	list * input = list_new();
	list * output = list_new();
	FILE * outfile = stdout;
	char * outfilename = 0;
	char * dirname = 0;
	int i = 1;
	
	/* Neat trick to get the thing to load the file called main. */
	list_tokenise_chars(input, "(include main)");
	
	for( ; ; ) {
		if(!argc[i]) break;
		
		// This switch says which directory to look for more source code
		// in. This is used for example by (include ...)
		if(!strcmp("-d", argc[i])) {
			i++;
			dirname = argc[i++];
			continue;
		}
		// This switch says which file in that directory to open first
		if(!strcmp("-i", argc[i])) {
			i++;
			langname = argc[i];
			load_file(input, dirname, argc[i++]);
			continue;
		}
		if(!strcmp("-I", argc[i])) {
			i++;
			load_file(input, "./", argc[i++]);
			continue;
		}
		if(!strcmp("-l", argc[i])) {
			i++;
			linkfile(output, argc[i++]);
			continue;
		}
		if(!strcmp("-o", argc[i])) {
			i++;
			outfilename = argc[i++];
			continue;
		}
	}
	if(!dirname) {
		fprintf(stderr, "You forgot to specify the directory where ");
		fprintf(stderr, "I should look for more source code.\n");
		list_free(input);
		list_free(output);
		exit(99);
	}

	/* First run all the (include) commands. They take a filename as the first
	 * argument, and loads that file, and appends it to the end of the input.
	 * In other words it's akin to #include in the C preprocessor. */
	include_pass(input, dirname);

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
	passdf(input, output, "df", outfilename);
	pass(input, output, "close", tclose);
	check_debug(output);
	check_wronginstruction(output);
	check_main_exists(output);
	check_remove_duplicate_tags(output);
	
	/* Dirty tricks */
	check_recursion(output);
	
	/* Optimisations */
	check_early_binding(output);
	check_removenops(output);

	/* Output */
	if(outfilename) {
		outfile = fopen(outfilename, "w");
	}
	printout(output, outfile);
	
	/* Check for ontological sanity */
	//check_ontology(output);
	
	/* Free stuff */
	list_free(output);
	list_free(input);
	return 0;
}
	

#include "../../list/list.h"
#include "compiler.h"

#include <stdlib.h>
#include <string.h>

int tagg(list * command, list * input, list * output) {
	
	char * fn = list_get_token(command, 2);
	char * num = list_get_token(command, 3);
	
	FILE * fp = fopen(fn, "r");
	if(!fp) {
		fprintf(stderr, "\tCouldn't open the file %s.\n", fn);
		exit(99);
	}
	list * file = list_new();
	list_tokenise_file(file, fp);
	fclose(fp);
	
	int i, j;
	for(i = 1; i<= file->length; i++) {
		list * sym = list_get_list(file, i);
		if(!sym) continue;
		char * symname = list_get_token(sym, 1);
		
		for(j = 2; j <= sym->length; j++) {
			
			list * symb = list_get_list(sym, j);
			if(!symb) continue;
			
			list * tag = list_find_list(symb, "tag");
			if(!tag) continue;
			
			if(!strcmp(num, list_get_token(tag, 3))) {
				
				list * newsym = list_find_list(output, symname);
				if(!newsym) {
					newsym = list_append_list(output);
					list_append_token(newsym, symname);
				}
				
				list_append_copy(list_append_list(newsym), symb);
				
				//fprintf(stderr, "\t(tag %s %s)\n\t\tyielded", fn, num);
				//list_fprettyprinter(stderr, newsym);
				//fprintf(stderr, "\n");
				return 0;
			}
		}
	}
	return 0;
}

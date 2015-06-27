#include "../../list/list.h"
#include <stdlib.h>
#include <string.h>

int tclose(list * command, list * input, list * output) {
	
	char * symb = list_get_token(command, 2);
	if(!symb) {
		fprintf(stderr, "\tSyntax: (close symb-name)\n");
		exit(99);
	}
	list * defs = list_find_list(output, symb);
	if(!defs) {
		fprintf(stderr, "\tI wanted to close %s but there's no such symbol.\n", symb);
		exit(98);
	}
	
	list_append_token(defs, "closed");

	return 0;
}

#include "../monad/monad.h"
#include "tranny.h"
#include <string.h>

int reduce_lit_in(monad * m, list * l) {
	list * lit = list_find_list(l, "lit");
	if(lit) {
		char * l = list_get_token(lit, 2);
		if(!strstr(m->intext + m->index, l)) {
			if(m->debug) {
				printf("reduce_lit_in skipped a rule because it had (lit %s) which is bound to fail.\n", l);
			}
			return 1;
		}
	}
	return 0;
}

int reduce_no_attest(monad * m, list * l) {
	list * attest = list_find_list(l, "attest");
	if(attest) {
		if(m->debug) {
			printf("reduce_no_attest skipped a rule because it had an attest instruction which is bound to fail.\n");
		}
		return 1;
	}
	return 0;
}

int reduce_no_makedef(monad * m, list * l) {
	list * attest = list_find_list(l, "makedef");
	if(attest) {
		if(m->debug) {
			printf("reduce_no_makedef skipped a rule because it had an makedef instruction.\n");
		}
		return 1;
	}
	return 0;
}

int reduce_varb(monad * m, list * l, char * nsname, int createflag) {
	
	list * binder = list_find_list(l, nsname);
	if(!binder) return 0;
	
	list * namespace = get_namespace(m, nsname, 0);
	if(!namespace) return 0;
	
	if(!check_vars(namespace, binder)) {
		if(m->debug) {
			printf("reduce_vars skipped a rule because it had ");
			list_prettyprinter(binder);
			printf(" which is bound to fail.\n");
		}
		return 1;
	}
	return 0;
}

int reduce_vars(monad * m, list * l) {
	
	if(reduce_varb(m, l, "rection", 1)) return 1;
	if(reduce_varb(m, l, "language", 0)) return 1;
	if(reduce_varb(m, l, "theta", 1)) return 1;
	if(reduce_varb(m, l, "seme", 0)) return 1;
	return 0;
}

int parse_reduce(monad * m, list * l) {
	
	if(reduce_lit_in(m, l)) return 1;
	if(reduce_no_attest(m, l)) return 1;
	if(reduce_vars(m, l)) return 1;
	
	return 0;
}

int generate_reduce(monad * m, list * l) {
	if(reduce_no_makedef(m, l)) return 1;
	if(reduce_no_attest(m, l)) return 1;
	if(reduce_vars(m, l)) return 1;
	
	return 0;
}

int learning_reduce(monad * m, list * l) {
	if(reduce_lit_in(m, l)) return 1;
	if(reduce_vars(m, l)) return 1;
	
	return 0;
}

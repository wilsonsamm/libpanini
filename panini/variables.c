#include "../monad/monad.h"
#include "../list/list.h"
#include "panini.h"
#include <string.h>
#include <stdlib.h>
char * negative(char * token);

/* One of the important datatypes in Panini is the variable. A variable has
 * three features:
 *  - The key,      or a name by which we can refer to a variable.
 *  - The polarity, either positive or negative.
 *  - The value,    some string which the variable is "bound to". This may not
 *                  contain any whitespace.
 * The key is always bound either positive or negative, but the value is
 * optional. If the variable has a value, that implies it is also positive.
 * The variable is immutable: once it has been bound, it is stuck with that
 * value forever (with some exceptions).
 *
 * Variables are kept in namespaces. Some namespaces are scoped, and others are
 * unscoped. If a namespace is scoped, that means you may name a scope, thereby
 * creating it, and bind variables in it. Variables in different scopes are
 * independant of each other, even if they have the same name.
 */

int checkvars(monad * m, list * varlist, int writeprotect) {

	/* Get the name of the namespace. */
	char * nsname = list_get_token(varlist, 1);

	/* Get the namespace where the variables should be bound */
	list * ns = get_namespace(m, nsname, !writeprotect);

	/* For each of the variables in the list of variables to bind, */
	int i;
	for(i = 2; i <= varlist->length; i++) {

		/* These variables will represent the variable as it looks in the
		 * list of variables to bind. */
		char * key = 0;
		char * value = 0;
		int polarity = 0; // A value of 1 will mean negative, 0 = positive.

		/* These variables will represent the variable as it looks in the
		 * namespace. */
		char * ovalue = 0;
		int opolarity = 0; // A value of 1 will mean negative, 0 = positive.
		int found = 0; // If the variable is already bound, we'll put 1 here
		
		/* Find the key of the variable to bind */
		if(list_get_token(varlist, i)) {
			key = list_get_token(varlist, i);
			if(key[0] == '-') {
				key = key + 1;
				polarity = 1;
			}
		} else {
			key = list_get_token(list_get_list(varlist, i), 1);
			value = list_get_token(list_get_list(varlist, i), 2);
		}

		/* If the namespace isn't there, then the variable can not be bound. */
		if(!ns) return 1;

		/* Find the variable in the namespace */
		if(list_contains(ns, key)) {
			found = 1;
		}
		if(list_contains_neg(ns, key)) {
			found = 1;
			opolarity = 1;
		}
		if(list_find_list(ns, key)) {
			ovalue = list_get_token(list_find_list(ns, key), 2);
			found = 1;
		}

		/* Check if the variable can be bound */
		if(found && value && ovalue && strcmp(value, ovalue)) return i;
		if(found && polarity != opolarity) return i;

		/* Unless write protection is turned on, or the variable already exists
		 * and already has a value, we can go ahead in append the variable to
		 * the namespace. */
		if(!writeprotect && value && ((!ovalue && found) || (!found))) {
			list * newv = list_append_list(ns);
			list_append_token(newv, key);
			list_append_token(newv, value);
			ovalue = value;
			found = 1;
		}

		/* Unless write protection is turned on, or the variable already exists
		 * and has no value yet, we can go ahead in append the variable to the
		 * namespace. */
		if(!writeprotect && !found && !value && !polarity) {
			list_append_token(ns, key);
		}
		if(!writeprotect && !found && !value && polarity) {
			char * n = malloc(strlen(key) + 2);
			strcpy(n, "-");
			strcat(n, key);
			list_append_token(ns, n);
			free(n);
		}
	}
	return 0;
}

#include "../monad/monad.h"
#include "../list/list.h"

#include "utils.h"

void format(list * tree) {
	char * hw;
	char * l;
	
	int i, j, k;
	
	for(i = 1; i <= tree->length; i++) {
		list * l1 = list_get_list(tree, i);
		if(!l1) continue;
		
		hw = list_get_token(l1, 1); 
		
		for(j = 2; j <= l1->length; j++) {
			
			list * l2 = list_get_list(l1, j);
			if(!l2) continue;
			
			l = list_get_token(l2, 1);
			
			char * tr = list_get_token(l2, 2);
			
			printf("%s (%s); %s", hw, l, tr);
			
			for(k = 3; k <= l2->length; k++) {
				printf(", %s", list_get_token(l2, k));
			}
			printf(".\n");
		}
	}
	
}

int tree(monad * m, void * tree) {
	/* First, find out what the headword is. */
	char * hw = m->intext;
	
	list * headword = list_find_list((list *)tree, hw);
	if(!headword) {
		headword = list_append_list((list *)tree);
		list_append_token(headword, hw);
	}
		
	/* Next, find out what label the entry should have */
	list * label = m->namespace;
	label = list_find_list(label, "clues");
	label = list_find_list(label, "label");
	char * lab = list_get_token(label, 2);
	
	list * l = list_find_list(headword, lab);
	if(!l) {
		l = list_append_list(headword);
		list_append_token(l, lab);
	}
	
	/* And last, append the translation. */
	char * tr = m->outtext;
	if(!list_contains(l, tr)) list_append_token(l, tr);
	
	return 0;
}

void translate(monad * m, char * language) {

	/* In the given language, */
	monad_rules(m, language);
	
	/* Generate all the translations for the headwords in this chain of monads. */
	monad_map(m, (int(*)(monad *, void *))set_stack, "(call Translation)", -1);
	monad_map(m, tranny_generate, (void *)0, -1);
	
	/* Strip the whitespace off the INTEXT and the OUTTEXT */
	monad_map(m, strp, (void *)0, -1);
	
	/* And then build a tree containing the headwords and all the labels, and the translations */
	list * t = list_new();
	monad_map(m, tree, t, -1);
	
	/* And then format the tree as a dictionary */
	format(t);
}

int main(int argc, char * argv[]) {
	
	monad * m = headwords(argv[1]);
	
	translate(m, argv[2]);
	return 0;
}

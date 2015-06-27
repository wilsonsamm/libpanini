#include "../../list/list.h"
#include <string.h>
#include <stdlib.h>

/* This function contains the procedures to check the ontological sanity of the produced tranny language module. The ontology is 
 * specified by the file "ontology.csv", which has the following fields:
 *  Category    - when you say (seme (x y)), x is the category, and y is the feature.
 *  Feature
 *  Negbind     - put 1 in here if the category can be bound negative (seme -category), otherwise put 0.
 *  Description - For Your Convenience (TM).
 */


/* This function loads the ontology from the file ontology.csv */
list * csvloader(char * fn) {
	FILE * csv = fopen(fn, "r");
	if(!csv) return 0;
	return list_tokenise_csv(csv);
}

char * get_xy(list * ontology, int x, int y) {
	list * row = list_get_list(ontology, y);
	if(!row) return 0;
	
	list * cell = list_get_list(row, x);
	if(!cell) return 0;
	
	return list_get_token(cell, 1);
}

/* Given a variable to bound, this checks it against the known ontology and returns: 
 *  0 if everything's OK,
 *  1 if the category is not known 
 *  2 if the category is known but can't be bound bound negative.
 */
int varcheckb(char * c, list * ontology) {
	int i = 0;
	char * varname;
	
	/* Check all the lines in the CSV */
	while(++i && (varname = get_xy(ontology, 1, i))) {
		
		char * nb = get_xy(ontology, 3, i);
		int negbind;
		if(nb) negbind = atoi(nb);
		else negbind = 0;
		
		/* If the variable name is being bound negative, then check that that's allowed 
		 * (that information is in column three; 0 for no and 1 for yes) */
		if(c[0] == '-' && !strcmp(varname, c + 1) &&  negbind) return 0;
		if(c[0] == '-' && !strcmp(varname, c + 1) && !negbind) return 2;
		
		/* If the variable is being bound positive (but not to a value), then check that the variable name is recognised */
		if(c[0] != '-')
			if(!strcmp(varname, c)) return 0;
		
	}
	
	return 1;
}

/* Given a variable to bound, this checks it against the known ontology and returns: 
 *  0 if everything's OK,
 *  1 if the category is known but doesn't have the feature 
 *  2 if the category is known but the feature isn't.
 *  3 if the feature somehow is not known
 */
int varcheckt(list * l, list * ontology) {
	int i;
	
	int categoryfound = 0;
	int featurefound = 0;
	
	char * category = list_get_token(l, 1);
	char * feature = list_get_token(l, 2);
	
	if(!feature) return 3;
	
	/* Check all the lines in the CSV */
	for(i = 2; i <= ontology->length; i++) {
		
		/* Look up the variable name */
		char * varname = get_xy(ontology, 1, i);
		if(!varname) continue;
		
		if(!strcmp(varname, category)) {
			categoryfound = 1;
			char * varval = get_xy(ontology, 2, i);
			if(varval) if(!strcmp(feature, varval)) featurefound = 1;
		}
		
		if(categoryfound && featurefound) return 0;
	}
	if(categoryfound && featurefound) return 0;
	if(categoryfound && !featurefound) return 2;
	
	return 1;
}


int checkrule(list * input, list * ontology) {
	int i, j;
	
	list * command;
		
	for(i = 1; i <= input->length; i++) {
		command = list_get_list(input, i);
		if(!command) continue;
		char * cname = list_get_token(command, 1);
		if(!cname) continue;
		if(strcmp(cname, "seme")) continue;
		
		for(j = 2; j <= command->length; j++) {
			list * varl = list_get_list(command, j);
			if(varl) {
				int retval = varcheckt(varl, ontology);
				
				if(!retval) continue;
				
				if(retval == 1) 
					fprintf(stderr,"\tThe \"%s\" variable is not a recognised category in the ontology!\n", list_get_token(varl, 1));
				
				if(retval == 2) 
					fprintf(stderr,"\tThe \"%s\" category does not have the feature \"%s\".\n", list_get_token(varl, 1), list_get_token(varl, 2));
				
				if(retval == 3) {
					fprintf(stderr,"\tHow can I bind (seme (%s %s))", list_get_token(varl, 1), list_get_token(varl, 2));
					fprintf(stderr,"\n");
				}
			}
			
			char * vart = list_get_token(command, j);
			if(vart) {
				int retval = varcheckb(vart, ontology);
				
				if(retval == 1)
					fprintf(stderr,"\tThe \"%s\" variable is not a recognised category in the ontology!\n", vart);
				if(retval == 2)
					fprintf(stderr,"\tThe \"%s\" variable is not something that can be bound negative!\n", vart);
			}
		}
	}
	return 0;
}

void check_ontology(list * output) {
	int i;
	char * dir = getenv("PANINI");
	char * fn = malloc(strlen(dir) + strlen("ontology.csv")+1);
	strcpy(fn,dir);
	strcat(fn, "ontology.csv");
	list * ontology = csvloader(fn);
	free(fn);
	
	for(i = 1; i<= output->length; i++) {
		int j;
		list * definitions = list_get_list(output, i);
		if(!definitions) continue;
		
		for(j = 2; j <= definitions->length; j++) {
			list * def = list_get_list(definitions, j);
			if(def)
				checkrule(def, ontology);
		}
	}
}

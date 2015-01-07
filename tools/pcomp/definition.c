#define LANGPATH "../languages/"
#include "../../list/list.h"
#include "compiler.h"

#include <stdlib.h>
#include <string.h>

#define PATH_MAX 2048

extern char * langname;

int define(char * name, list * definition, list * output, char * outfn) {
	
	static int num = 0;
	
	list * rule_list = list_find_list(output, name);
	if(!rule_list) {
		rule_list = list_append_list(output);
		list_append_token(rule_list, name);
	}
	
	list * new_pattern = list_append_list(rule_list);
	
	/* We'll add a sandhi command in here to the beginning of the rule.
	 * It's harmless on it's own but simplifies the sandhi routines in a 
	 * later pass. */
	list * sandhi = list_append_list(new_pattern);
	list_append_token(sandhi, "sandhi");
	
	/* It is theoretically faster to bind variables as early as possible, so we
	 * will add (seme), (language) and (rection) here. In a later pass, we will 
	 * move any variables bound later on. This will (hopefully!) speed the program
	 * up a little. */
//	list * rection = list_append_list(new_pattern);
//	list_append_token(rection, "rection");
//	list * seme = list_append_list(new_pattern);
//	list_append_token(seme, "seme");
//	list * language = list_append_list(new_pattern);
//	list_append_token(language, "language");
	
	/* We'll add a (brake) command in here to the beginning of the rule.
	 * In a later pass it gets removed from patterns that must not have 
	 * it. */
	list * brake = list_append_list(new_pattern);
	list_append_token(brake, "brake");	
	list_append_copy(new_pattern, definition);

	list * tag = list_find_list(new_pattern, "tag");
	if(tag) {
		if(!outfn) {
			fprintf(stderr, "\tBecause this source code uses (tag), ");
			fprintf(stderr, "the -o option is required.\n");
			exit(99);
		}
			
		char cwd[PATH_MAX];
		char strnum[20];
		getcwd(cwd, PATH_MAX - strlen(outfn));
		strcat(cwd, "/");
		strcat(cwd, outfn);
		snprintf(strnum, 20, "%d", num++);
		
		list_append_token(tag, cwd);
		list_append_token(tag, strnum);
	}
	
	return 0;
}

int df(list * command, list * input, list * output, char * outfn) {
	int retval;
	
	char * name = list_get_token(command, 2);
	//fprintf(stderr, "\tNow defining %s.\n", name);
	
	list * rule = list_new();
	int i;
	for(i = 1; i <= command->length; i++) {
		list * instr = list_get_list(command, i);
		if(instr) list_append_copy(list_append_list(rule), instr);
		if(instr && !strcmp(list_get_token(instr, 1), "lit")) {
			list * sandhi = list_append_list(rule);
			list_append_token(sandhi, "sandhi");
		}
	}
	
	retval = define(name, rule, output, outfn);
	list_free(rule);
	

	list_append_token(command, "nop");

	return retval;
}

int segment(list * command, list * input, list * output) {
	
	list * rule = list_append_list(input);
	list_append_token(rule, "df");
	list_append_token(rule, "Segment");
	
	int i;
	for(i = 1; i <= command->length; i++) {
		list * instr = list_get_list(command, i);
		
		if(!instr) continue;
		
		if(!strcmp(list_get_token(instr, 1), "underlying")) list_rename(instr, "flags");
		if(!strcmp(list_get_token(instr, 1), "surface"))    list_rename(instr, "lit");
		
		list_append_copy(list_append_list(rule), instr);
		
	}

	return 0;
}

int df(list * command, list * input, list * output);
int define(char * name, list * definition, list * output);
int segment(list * command, list * input, list * output);
int sandhi_initial(list * command, list * input, list * output);
int sandhi_final(list * command, list * input, list * output);
int for_(list * command, list * input, list * output);
void check_ontology(list * output);

int check_syntax1(list * input);
int check_debug(list * output);
int check_wronginstruction(list * output);
int check_recursion(list * output);
int check_removenops(list * output);
int check_removetags(list * output);
int check_main_exists(list * output);
int check_deprecated(list * output);
int check_space(list * output);
int check_early_binding(list * output);

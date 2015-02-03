#define NONE  0
#define HAPPY 1
#define SAD   2
#define DONE  3
#define DOTS  4

extern char * currentline;

void count_lines(FILE * fp);
void progpc(int smile, char * text);
int nextline(FILE * fp);
char * getfield(int n);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panini.h>

int main(int argc, char * argv[]) {

	FILE * all = fopen("AllWords.txt", "w");
	monad * m = monad_new();
	
	monad_rules(m, argv[1]);
	panini_generate(m, argv[2], 0, 5);
	monad_map(m, kill_identical_outtexts, 0, 5);
	monad_map(m, print_out, all, 5);
	fclose(all);
	return 0;
}

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <panini.h>
void genders_generate(monad * m, char * gname) {
	monad * n = monad_duplicate(m);
	char exec[1024];
	snprintf(exec, 1024, "(call noun %s lemma)", gname);

	panini_generate(n, exec, 0, 1);
	FILE * gfile = fopen(".gender", "w+");
	monad_map(n, print_out, gfile, -1);
	rewind(gfile);
	
	FILE * out = fopen("genders.csv", "a+");
	for(;;) {
		char lemma[1024];
		int offs = 0;
		int c = fgetc(gfile);
		if(c == EOF) break;
		
		/* Read in one word (all words are on separate lines) */
		while(c != '\n') {
			lemma[offs++] = c;
			lemma[offs] = 0;
			c = fgetc(gfile);
		}
		if(lemma[offs-1] == ' ') lemma[offs-1] = '\0';
		
		/* And then print that word to the CSV with gender information. */
		fprintf(out, "\"%s\",\"%s\"\n", lemma, gname);
		offs = 0;
		lemma[0] = '\0';
	}
	monad_free(n);
	fclose(gfile);
	fclose(out);
	unlink(".gender");
}

int genders(char * langname) {
	if(!langname) {
		printf("Usage:\n $ flashcards genders [LANGUAGE]\n");
		printf("possible languages include czech, ojibwe...");
		return 1;
	}
	
	unlink("genders.csv");
	
	monad * m = monad_new();
	
	if(!strcmp(langname, "czech")) {
		monad_rules(m, "czech");
		
		genders_generate(m, "masculine animate");
		genders_generate(m, "masculine inanimate");
		genders_generate(m, "feminine");
		genders_generate(m, "neuter");
		
	} else if(!strcmp(langname, "ojibwe")) {
		monad_rules(m, "algonquian");
		panini_parse(m, "(language (language ojibwe))", "", 0, 0, 0);
		genders_generate(m, "animate");
		genders_generate(m, "inanimate");
		
	} else if(!strcmp(langname, "cree")) {
		monad_rules(m, "algonquian");
		panini_parse(m, "(language (language cree))", "", 0, 0, 0);
		genders_generate(m, "animate");
		genders_generate(m, "inanimate");
	} else {
		printf("Usage:\n $ flashcards genders [LANGUAGE]");
		printf("unknown language. ");
		printf("possible languages include czech, ojibwe...");
		monad_free(m);
		return 1;
	}
	monad_free(m);
	return 0;
}
		

int main(int argc, char * argv[]) {
	if(argc == 1) {
		printf("Usage:\n $ flashcards PROGRAM [OPTIONS ...]\n");
		printf("Available programs are:\n - vocab\n - genders\n");
		return 0;
	}
	
//	if(strcmp(argv[1], "vocab"))
//		return vocab(argv[2], vocab[3]);
	
	if(!strcmp(argv[1], "genders"))
		return genders(argv[2]);
	printf("Wrong program\n");
	
	return 1;
}

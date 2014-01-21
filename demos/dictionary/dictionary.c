
#include <tranny.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// To use this program, type:
//  $ dict srclang dstlang

int hwcount;
int trcount;

char * from;
char * From;
char * to;
char * To;

char * lemma;			/* How to generate the lemma */
char * translations;	/* How to translate the lemma */

char * label;			/* Each headword could be labelled (noun) or (verb) or something */

FILE * temp;
FILE * output;
FILE * headwords;

void helpfulinfo(char * name) {
	fprintf(stderr, "The %s dictionary has been compiled,\n", name);
	fprintf(stderr, "\twith %d headwords and ", hwcount);
	fprintf(stderr, "%d translations.\n", trcount);
}

void open_output(char * fname) {
	output = fopen(fname, "w+");
	if(!output) {
		printf("Could not open the output file %s!\n", fname);
		exit(1);
	}
}

char * next_word(FILE *fp) {
	/* In this buffer we'll store the word to read in fromt he file. */
	char w[1024];
	
	/* We'll read the word in from the file. */
	int c;
	int i = 0;
	while((c = fgetc(fp))) {
		if(c == EOF) return 0;
		if(i>1024) return 0;
		if(c == '\n') break;
		
		w[i++] = (char)c;
	}
	w[i] = 0;
	
	while(w[strlen(w) - 1] == ' ')
		w[strlen(w) - 1] = '\0';
	
	return strdup(w);
}

/* This function will generate all the lemmas in the language. */
int generate_lemmas() {
	
	printf("Generating %s %s lemmas ... \n", From, label);
	fflush(stdout);
	
	/* Open the headwords file. This is what we'll use to store the headwords */
	headwords = fopen("headwords", "w");
	
	/* create a new monad. */
	monad * m = monad_new();
	
	/* load the appropriate languages */
	monad_rules(m, from);
	
	/* Generate headwords */
	monad_map(m, set_stack, lemma, 0);
	monad_map(m, tranny_gowild, (void *)0, 0);
	monad_map(m, kill_identical_outtexts, (void *)0, 0);
	
	/* Print all the headwords out to that file. */
	monad_map(m, print_out, headwords, 0);
	fclose(headwords);
	
	/* free the monads. */
	monad_free(m);
	return 0;
}

void format(char * lemma) {
	FILE * tmp = fopen("temp", "r");
	
	char * l;
	l = next_word(tmp);
	if(!l) return;
	
	hwcount++;
	fprintf(output, "%s %s; %s", lemma, label, l);
	
	while((l = next_word(tmp))) {
		fprintf(output,", %s", l);
		trcount++;
	}
	fprintf(output, ".\n");
}

void generate_translations() {
	
	char * l; /* This variable will store each word as it's read in from the headwords file */
	
	headwords = fopen("headwords", "r");
	
	while((l = next_word(headwords))) {
		
		FILE * tmp = fopen("temp", "w");
		
		monad * m = monad_new();
		monad_rules(m, from);
		
		/* Get the meaning of the headwords */
//		printf("Interpreting \"%s\" as %s.\n", l, label);
		monad_map(m, set_stack, lemma, 0);
		monad_map(m, set_intext, l, 0);
		if(!monad_map(m, tranny_parse, (void *)0, 0)) continue;
		
//		printf("Translating \"%s\" to %s.\n", l, To);
		fflush(stdout);
	
		/* And then make translations, printing them to the temporary file. */
		monad_rules(m, to);
		monad_map(m, set_stack, translations, 0);
		if(monad_map(m, tranny_generate, (void *)0, 0)) {
			monad_map(m, kill_identical_outtexts, (void*)0, 0);
			monad_map(m, print_out, tmp, 0);
		} else {
//			printf("There is no %s word for \"%s\"!\n", To, l);
		}
		
		monad_free(m);
		fclose(tmp);
		
		format(l);
		free(l);
	}
}	

void entries() {
	generate_lemmas();
	generate_translations();
}

void swahili_english() {
	from = "swahili";
	From = "Swahili";
	to = "english";
	To = "English";
	
	open_output("swahili-english.txt");

	/* Swahili nouns correspond to English nouns. */
	lemma = "(constituent noun class-1 singular)";
	translations = "(constituent noun)";
	label = "(m/wa)";
	entries();

	/* Swahili verbstems correspond to English verbs. */
	lemma = "(constituent verbstem lemma)";
	translations = "(constituent verbroot)";
	label = "(v.)";
	entries();
	
	helpfulinfo("Swahili - English");
}

void english_ainu() {
	from = "english";
	From = "English";
	to = "ainu";
	To = "Ainu";
	
	open_output("english-ainu.txt");

	/* English nouns correspond to Ainu nouns. */
	lemma = "(constituent nounstem)";
	translations = "(constituent nounstem)";
	label = "(n.)";
	entries();

	/* English verbstems correspond to Ainu verbstems. */
	lemma = "(constituent verbstem)";
	translations = "(constituent verbstem)";
	label = "(v.)";
	entries();
	
	helpfulinfo("English - Ainu");
}

void ainu_english() {
	from = "ainu";
	From = "Ainu";
	to = "english";
	To = "English";
	
	open_output("ainu-english.txt");

	/* English nouns correspond to Ainu nouns. */
	lemma = "(constituent nounstem)";
	translations = "(constituent nounstem)";
	label = "(n.)";
	entries();

	/* English verbstems correspond to Ainu verbstems. */
	lemma = "(constituent verbstem)";
	translations = "(constituent verbstem)";
	label = "(v.)";
	entries();
	
	helpfulinfo("Ainu - English");
}	

void nahuatl_english() {
	from = "nahuatl";
	From = "Nahuatl";
	to = "english";
	To = "English";
	
	open_output("nahuatl-english.txt");

	/* Nahuatl nouns correspond to English ones. */
	lemma = "(seme (number singular)) (constituent noun)";
	translations = "(constituent noun)";
	label = "(n.)";
	entries();

	/* Nahuatl verbstems correspond to English verbstems. */
	lemma = "(constituent verbstem)";
	translations = "(constituent verbroot)";
	label = "(v.)";
	entries();
	
	/* Nahuatl stative verbs may correspond to English adjectives. */
	lemma = "(constituent verbstem stative)";
	translations = "(constituent adjective)";
	label = "(v. st.)";
	entries();
	helpfulinfo("Nahuatl - English");
}

int main(int argc, char * argv[]) {
	hwcount = 0;
	trcount = 0;
	
	if(!strcmp(argv[1], "swahili-english")) swahili_english();
	else if(!strcmp(argv[1], "ainu-english")) ainu_english();
	else if(!strcmp(argv[1], "english-ainu")) english_ainu();
	else if(!strcmp(argv[1], "nahuatl-english")) nahuatl_english();
	else {
		printf("I was called with the argument \"%s\", which I don't know.\n", argv[1]);
		exit(1);
	}
	
	unlink("headwords");
	unlink("temp");
	
	return 1;
}

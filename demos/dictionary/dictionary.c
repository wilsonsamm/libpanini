#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tranny.h>

// To use this program, type:
//  $ dict srclang dstlang

#define NOM	1	// Must be first
#define ADJ 2
#define VRB 3	// Must be last

#define LEMMA  1 // The tranny code to generate a lemma (lemma is another word for headword)
#define LEARN  2 // The tranny code to learn a word
#define LABEL  3 // The short label that comes after a headword in a dictionary (eg. (n.) for noun)
#define VLABEL 4 // The English word for the part of speech (eg. verb, adjective)


int hwcount;
int trcount;

char * from;
char * to;

void d_info(char * name) {
	fprintf(stderr, "The %s dictionary has been compiled, ", name);
	fprintf(stderr, "with %d headwords and ", hwcount);
	fprintf(stderr, "%d translations.\n", trcount);
}

void l_info(char * name) {
	fprintf(stderr, "%d %s words have been learned by interrogating the user.\n", trcount, to);
}

FILE * open_output(char * fname) {
	FILE * output = fopen(fname, "w+");
	if(!output) {
		printf("Could not open the output file %s!\n", fname);
		exit(1);
	}
	return output;
}

char * next_word(FILE *fp) {
	/* In this buffer we'll store the word to read in from the file. */
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
int generate_lemmas(char * lemma) {
	
	/* Open the headwords file. This is what we'll use to store the headwords */
	FILE * headwords = fopen("headwords", "w");
	
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

void format(char * lemma, char * label) {
	FILE * tmp = fopen("temp", "r");
	
	char * l;
	l = next_word(tmp);
	if(!l) return;
	
	hwcount++;
	trcount++;
	printf("%s %s; %s", lemma, label, l);
	
	while((l = next_word(tmp))) {
		printf(", %s", l);
		trcount++;
	}
	printf(".\n");
}

void generate_translations(char * tranny_from, char * tranny_to, char * label) {
        
	FILE * headwords = fopen("headwords", "r");
	
	char * hw;
	
	while((hw = next_word(headwords))) {
		FILE * tmp = fopen("temp", "w");
		monad * m = monad_new();
		monad_rules(m, from);
		
		/* Get the meaning of each headword */
		monad_map(m, set_stack, tranny_from, -1);
		monad_map(m, set_intext, hw, -1);
		if(!monad_map(m, tranny_parse, (void *)0, 0)) continue;
	               
		/* And then make translations, printing them to the temporary file. */
		monad_rules(m, to);
		monad_map(m, set_stack, tranny_to, -1);
		if(monad_map(m, tranny_generate, (void *)0, 0)) {
			monad_map(m, kill_identical_outtexts, (void*)0, -1);
			monad_map(m, print_out, tmp, -1);
		}
		monad_free(m);
		fclose(tmp);
		format(hw, label);
		unlink("temp");
	}
}

void ask_for_translations(char * tranny_from, char * tranny_to, char * learn, char * label_from, char * label_to) {
	     
	FILE * headwords = fopen("headwords", "r");
	
	FILE * output = fopen("learned.out", "a+");
	
	char * hw;
	
	while((hw = next_word(headwords))) {
		monad * m = monad_new();
		monad_rules(m, from);
		
		/* Get the meaning of the headwords, ... */
		monad_map(m, set_intext, hw, 0);
		monad_map(m, set_stack, tranny_from, 0);
			
		if(!monad_map(m, tranny_parse, (void *)0, 0)) continue;
		monad_map(m, remove_ns, "theta", -1);
		monad_map(m, remove_ns, "rection", -1);
			
		/* Do we already know how to translate this word? then skip it. */
		monad_rules(m, to);
		monad_map(m, set_stack, tranny_to, -1);
		if(monad_map(m, tranny_generate, (void *)0, -1)) continue;
		
		/* That test actually destroys the state of the monad, so I suppose we'll have to parse it again. */
		monad_free(m);
		m = monad_new();
		monad_rules(m, from);
		monad_map(m, set_stack, tranny_from, -1);
		monad_map(m, set_intext, hw, -1);
		if(!monad_map(m, tranny_parse, (void *)0, -1)) {
			fprintf(stderr, "Couldn't reparse '%s'. Wat?\n", hw);
			continue;
		} 
			
		/* Otherwise, ask the user how to translate it. */
		printf("\"%s\" is a %s %s. ", hw, from, label_from);
		printf("What is a %s %s for this? ", to, label_to);
		fflush(stdout);
		char string[256];
		fgets(string, 256, stdin);
		string[strlen(string)-1] = '\0'; // Remove trailing newline
		if(!strlen(string)) continue;
		
		/* Try to understand the translation */
		monad_rules(m, to);
		monad_map(m, set_stack, learn, -1);
		monad_map(m, set_intext, string, -1);
		if(monad_map(m, tranny_learn, output, 1)) trcount++;

		monad_free(m);
	}
	fclose(output);
}


char * ainu(int class, int ret) {
	switch(class) {
		case NOM:
			switch(ret) {
				case LEMMA:  return "(constituent noun)";
				case LABEL:  return "(n.)";
				case VLABEL: return "nounstem";
				case LEARN:  return "(constituent noun learn)";
			}
			return 0;
		case VRB:
			switch(ret) {
				case LEMMA:  return "(constituent verbstem active)";
				case LABEL:  return "(v.)";
				case VLABEL: return "verbstem";
			}
			return 0;
		case ADJ:
			switch(ret) {
				case LEMMA:  return "(constituent verbstem stative)";
				case LABEL:  return "(v. st.)";
				case VLABEL: return "stative verbstem";
			}
			return 0;
	}
	return 0;
}

char * nahuatl(int class, int ret) {
		switch(class) {
		case NOM:
			switch(ret) {
				case LEMMA:  return "(rection (number singular)) (constituent noun)";
				case LABEL:  return "(n.)";
				case VLABEL: return "nounstem";
			}
			return 0;
		case VRB:
			switch(ret) {
				case LEMMA:  return "(constituent verbstem active)";
				case LABEL:  return "(v.)";
				case VLABEL: return "verbstem";
			}
			return 0;
		case ADJ:
			switch(ret) {
				case LEMMA:  return "(constituent verbstem stative)";
				case LABEL:  return "(v. st.)";
				case VLABEL: return "stative verbstem";
			}
			return 0;
	}
	return 0;
}

char * english(int class, int ret) {
	switch(class) {
		case NOM:
			switch(ret) {
				case LEMMA:	return "(rection (number singular)) (constituent noun)";
				case LEARN:	return "(constituent noun learn)";
				case LABEL:		return "(n.)";
				case VLABEL:	return "noun";
			}
			return 0;
		case VRB:
			switch(ret) {
				case LEMMA:	return "(constituent verbstem)";
				case LABEL:		return "(v.)";
				case VLABEL:	return "verb";
			}
			return 0;
		case ADJ:
			switch(ret) {
				case LEMMA:	return "(constituent adjective)";
				case LEARN: return "(constituent adjective learn)";
				case LABEL:		return "(adj.)";
				case VLABEL:	return "adjective";
			}
			return 0;
	}
	return 0;
}

char * czech(int class, int ret) {
	switch(class) {
		case NOM:
			return 0;
		case VRB:
			switch(ret) {
				case LEMMA:	return "(constituent verb infinitive)";
				case LABEL:		return "(v.)";
				case VLABEL:	return "verb";
			}
			return 0;
		case ADJ:
			return 0;
	}
	return 0;
}
char * call_language(char * which, int class, int what) {
	if(!strcmp(which, "nahuatl")) return nahuatl(class, what);
	if(!strcmp(which, "english")) return english(class, what);
	if(!strcmp(which, "ainu"))    return ainu(class, what);
	if(!strcmp(which, "czech"))   return czech(class, what);
	return 0;
}	

void bilingual() {
	
	int i;
	for(i = NOM; i<=VRB; i++) {
		/* Generate lemmas */
		char * tranny_from = call_language(from, i, LEMMA);
		char * label = call_language(from, i, LABEL);
		char * tranny_to = call_language(to, i, LEMMA);
		
		generate_lemmas(tranny_from);
		generate_translations(tranny_from, tranny_to, label);
	}
}

void learn() {
	
	int i;
	for(i = NOM; i<=VRB; i++) {
		
		/* Generate lemmas */
		char * tranny_from = call_language(from, i, LEMMA);
		char * tranny_to = call_language(to, i, LEMMA);
		char * learn = call_language(to, i, LEARN);
		char * label_from = call_language(from, i, VLABEL);
		char * label_to = call_language(to, i, VLABEL);
		
		if(!tranny_from) continue;
		if(!tranny_to) continue;
		if(!learn) continue;
		if(!label_from) continue;
		if(!label_to) continue;
		
		generate_lemmas(tranny_from);
		ask_for_translations(tranny_from, tranny_to, learn, label_from, label_to);
	}
}

void print_lemmas() {
	FILE * headwords = fopen("headwords", "r");
	
	char * hw;
	while((hw = next_word(headwords))) printf("%s ", hw);
}

void wordlist() {	
	int i;
	for(i = NOM; i<=VRB; i++) {
		
		/* Generate lemmas */
		char * l = call_language(from, i, LEMMA);
		if(!l) continue;
		generate_lemmas(l);
		print_lemmas();
	}
}

int main(int argc, char * argv[]) {
	hwcount = 0;
	trcount = 0;
	
	if(!strcmp(argv[1], "bilingual")) {
		from = argv[2];
		to = argv[3];
		bilingual();
	}
	
	else if(!strcmp(argv[1], "learn")) {
		from = argv[2];
		to = argv[3];
		learn();
	}
	
	else if(!strcmp(argv[1], "wordlist")) {
		from = argv[2];
		wordlist();
	}
	
	else {
		printf("I was called with the argument \"%s\", which I don't know.\n", argv[1]);
		exit(1);
	}
	
	unlink("headwords");
	unlink("temp");
	
	return 0;
}

#include "flash.h"

int main(int argc, char * argv[]) {
	if(argc == 1) {
		printf("Usage:\n $ flashcards PROGRAM [OPTIONS ...]\n");
		printf("Available programs are:\n - vocab\n - genders\n");
		return 0;
	}
	
	if(!strcmp(argv[1], "vocab"))
		return vocab(argv[2], argv[3], argv[4]);
	
	if(!strcmp(argv[1], "genders"))
		return genders(argv[2]);
	printf("Wrong program\n");
	
	return 1;
}

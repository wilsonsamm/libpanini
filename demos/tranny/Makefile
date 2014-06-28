all: clean tranny

tranny: tranny.o languages.o
	gcc -o tranny tranny.o languages.o -lpanini
	
tranny.o: tranny.c
	gcc -g -c -Wall tranny.c
	
languages.o: languages.c
	gcc -g -c -Wall languages.c
	
clean:
	rm -rf tranny tranny.o languages.o gmon.out
	
install:
	cp tranny /usr/bin/

uninstall:
	rm -rf /usr/bin/tranny

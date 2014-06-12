all: clean tranny
tranny: tranny.o
	gcc -o tranny tranny.o -lpanini
	
tranny.o: tranny.c
	gcc -g -c -Wall tranny.c -pg
	
clean:
	rm -rf tranny tranny.o gmon.out
	
install:
	cp tranny /usr/bin/

uninstall:
	rm -rf /usr/bin/tranny

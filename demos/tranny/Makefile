all: clean tranny
tranny: tranny.o
	gcc -o tranny tranny.o -ltranny
	
tranny.o: tranny.c
	gcc -g -c -Wall tranny.c
	
clean:
	rm -rf tranny tranny.o
	
install:
	cp tranny /usr/bin/

uninstall:
	rm -rf /usr/bin/tranny

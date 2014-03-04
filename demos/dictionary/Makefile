all: clean dictionary 
dictionary: dictionary.o
	gcc -o dictionary dictionary.o -ltranny
	
dictionary.o: dictionary.c
	gcc -g -c -Wall dictionary.c

clean:
	rm -rf dictionary dictionary.o wordlist.o wordlist *.txt *-learned wordlist-*

install:
	cp tranny /usr/bin/

uninstall:
	rm -rf /usr/bin/tranny

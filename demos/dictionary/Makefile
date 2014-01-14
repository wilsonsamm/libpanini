all: clean dictionary wordlist
dictionary: dictionary.o
	gcc -o dictionary dictionary.o -ltranny
	
wordlist: wordlist.o
	gcc -o wordlist wordlist.o -ltranny

dictionary.o: dictionary.c
	gcc -g -c -Wall dictionary.c

wordlist.o: wordlist.c
	gcc -g -c -Wall wordlist.c

clean:
	rm -rf dictionary dictionary.o wordlist.o wordlist
	
install:
	cp tranny /usr/bin/

uninstall:
	rm -rf /usr/bin/tranny

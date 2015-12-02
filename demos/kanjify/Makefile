all: clean kanjify

kanjify: kanjify.o
	gcc -o kanjify kanjify.o -lpanini
	
kanjify.o: main.c
	gcc -g -c -Wall -o kanjify.o main.c
	
clean:
	rm -rf kanjify *.o gmon.out
	
install:
	cp kanjify /usr/bin/

uninstall:
	rm -rf /usr/bin/kanjify

deepclean: clean

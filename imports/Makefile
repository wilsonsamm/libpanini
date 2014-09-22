
all: ekan

ekan.out: ekan
	./ekan > ekan.out

readword.o : readword.c read.h
	gcc -g -c -pg -ggdb readword.c
	
reading.o : reading.c reading.h
	gcc -g -c -pg -ggdb reading.c

ekan: kanjidic.o edict.o readword.o 
	gcc -pg -o ekan edict.o kanjidic.o readword.o -lpanini

kanjidic.o: kanjidic.c
	gcc -g -c -pg -ggdb kanjidic.c

kanji: kanji.o readword.o reading.o
	gcc -ggdb -o kanji kanji.o readword.o reading.o

edict.o: edict.c
	gcc -g -c -ggdb edict.c

edict: edict.o readword.o
	gcc -o edict edict.o readword.o -lpanini

deepclean: clean
	rm -rf kanjidic

clean:
	rm -rf *.o ekan *.txt core
	rm -rf edict ekan.out gmon.out 

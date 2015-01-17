
all: ekan

ekan.out: ekan
	./get-dicts
	./ekan > ekan.out

readword.o : readword.c read.h
	gcc $(CCOPTS) readword.c
	
reading.o : reading.c reading.h
	gcc $(CCOPTS) reading.c

ekan: kanjidic.o edict.o readword.o 
	gcc -pg -o ekan edict.o kanjidic.o readword.o $(PANINI)/libpanini.a

kanjidic.o: kanjidic.c
	gcc $(CCOPTS) kanjidic.c

kanji: kanji.o readword.o reading.o
	gcc -o kanji kanji.o readword.o reading.o

edict.o: edict.c
	gcc $(CCOPTS) edict.c

edict: edict.o readword.o
	gcc -o edict edict.o readword.o $(PANINI)/libpanini.a

clean:
	rm -rf *.o ekan *.txt *.gz core
	rm -rf edict ekan.out gmon.out 
	rm -rf kanjidic

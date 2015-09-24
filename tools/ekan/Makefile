
all: ekan

ekan.out: ekan
	./get-dicts
	./ekan > ekan.out

readword.o : readword.c read.h
	$(CCOMPILER) $(CCOPTS) readword.c
	
reading.o : reading.c reading.h
	$(CCOMPILER) $(CCOPTS) reading.c

ekan: kanjidic.o edict.o readword.o 
	$(CCOMPILER) -o ekan edict.o kanjidic.o readword.o -lpanini

kanjidic.o: kanjidic.c
	$(CCOMPILER) $(CCOPTS) kanjidic.c

kanji: kanji.o readword.o reading.o
	$(CCOMPILER) -o kanji kanji.o readword.o reading.o

edict.o: edict.c
	$(CCOMPILER) $(CCOPTS) edict.c

edict: edict.o readword.o
	$(CCOMPILER) -o edict edict.o readword.o $(PANINI)/libpanini.a

clean:
	rm -rf *.o ekan *.txt *.gz core
	rm -rf edict ekan.out gmon.out 
	rm -rf kanjidic

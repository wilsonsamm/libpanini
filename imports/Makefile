
all: ekan

kanjidic.locale: 
	@echo Converting kanjidic to the current locale.
	@cat /usr/share/edict/kanjidic | iconv --from EUC-JISX0213 > kanjidic.locale

edict.locale: 
	@echo Converting edict to the current locale.
	@cat /usr/share/edict/edict | iconv --from EUC-JP > edict.locale

ekan.out: ekan
	./ekan > ekan.out

readword.o : readword.c read.h
	gcc -g -c -pg -ggdb readword.c
	
reading.o : reading.c reading.h
	gcc -g -c -pg -ggdb reading.c

ekan: kanjidic.o edict.o readword.o kanjidic.locale edict.locale
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
	rm -rf *.o ekan
	rm -rf kanjidic kanjidic.locale edict edict.locale edict.locale.tmp 
	rm -rf ekan.out gmon.out 

panini-kanji: kanji kanjidic.locale
	./kanji | sort | uniq > panini-kanji

panini-edict: edict edict.locale
	./edict | sort | uniq > panini-edict

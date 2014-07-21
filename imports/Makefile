
all: panini-kanji panini-edict

kanjidic.locale: 
	@echo Converting kanjidic to the current locale.
	@cat /usr/share/edict/kanjidic | iconv --from EUC-JISX0213 > kanjidic.locale

edict.locale: 
	@echo Converting edict to the current locale.
	@cat /usr/share/edict/edict | iconv --from EUC-JP > edict.locale

readword.o : readword.c read.h
	gcc -g -c -ggdb readword.c

kanji.o: kanji.c
	gcc -g -c -ggdb kanji.c

kanji: kanji.o readword.o
	gcc -ggdb -o kanji kanji.o readword.o

edict.o: edict.c
	gcc -g -c -ggdb edict.c

edict: edict.o readword.o
	gcc -o edict edict.o readword.o -lpanini

deepclean: clean
	rm -rf kanjidic

clean:
	rm -rf readword.o kanji.o edict.o kanjidic kanjidic.locale panini-kanji edict edict.locale kanji

panini-kanji: kanji kanjidic.locale
	./kanji | sort | uniq > panini-kanji

panini-edict: edict edict.locale
	./edict | sort | uniq > panini-edict

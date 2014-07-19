
all: panini-kanji

kanjidic:
	@echo Copying /usr/share/edict/kanjidic to here
	@cp /usr/share/edict/kanjidic ./

kanjidic.locale: kanjidic
	@echo Converting kanjidic to the current locale.
	@cat kanjidic | iconv --from EUC-JISX0213 > kanjidic.locale

edict:
	@echo Copying /usr/share/edict/edict to here
	@cp /usr/share/edict/edict ./

edict.locale: kanjidic
	@echo Converting edict to the current locale.
	@cat edict | iconv --from EUC-JP > edict.locale

readword.o : readword.c read.h
	gcc -g -c -ggdb readword.c

kanji.o: kanji.c
	gcc -g -c -ggdb kanji.c

kanji: kanji.o readword.o
	gcc -ggdb -o kanji kanji.o readword.o

deepclean: clean
	rm -rf kanjidic

clean:
	rm -rf readword.o kanji.o kanjidic kanjidic.locale panini-kanji edict edict.locale kanji

panini-kanji: kanji kanjidic.locale
	./kanji | uniq > panini-kanji

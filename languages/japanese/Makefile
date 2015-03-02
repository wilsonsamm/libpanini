japanese: $(PANINI_EKAN)
	echo Compiling japanese.
	cp ekan.out src/ekan
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > japanese

kanjidic.txt:
	cat /usr/share/edict/kanjidic | iconv --from EUC-JISX0213 > kanjidic.txt

edict.txt:
	cat /usr/share/edict/edict | iconv --from EUC-JISX0213 > edict.txt

ekan.out: kanjidic.txt edict.txt
	echo Importing EDICT and KANJIDIC.
	$(PANINI)/tools/ekan/ekan > ekan.out

clean:
	rm -f japanese core ekan.out gmon.out edict kanjidic *.txt src/ekan
	rm -f kanjidic.gz

install:
	cp japanese /usr/panini/languages

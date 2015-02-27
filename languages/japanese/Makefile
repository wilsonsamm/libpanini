ekan.out:
	echo Importing EDICT and KANJIDIC.
	./get-dicts.sh
	$(PANINI)/tools/ekan/ekan > ekan.out

japanese: ekan.out
	echo Compiling japanese.
	cp ekan.out src/ekan
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > japanese

clean:
	rm -f japanese core ekan.out gmon.out edict kanjidic *.txt src/ekan
	rm -f kanjidic.gz

install:
	cp japanese /usr/panini/languages

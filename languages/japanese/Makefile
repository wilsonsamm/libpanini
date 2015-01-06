ekan.out: 
	./get-dicts.sh
	$(PANINI)/tools/ekan/ekan > ekan.out

japanese: ekan.out
	cp ekan.out src/ekan
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > japanese

clean:
	rm -f japanese core ekan.out gmon.out *.txt src/ekan

install:
	cp japanese /usr/panini/languages

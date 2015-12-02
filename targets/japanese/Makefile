japanese: 
	touch $(PANINI)/tools/ekan/ekan.out
	cat src/ekan.out $(PANINI)/tools/ekan/ekan.out > ekan.out
	mv ekan.out src/ekan.out
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > japanese

clean:
	rm -f japanese core ekan.out gmon.out
	rm -f edict kanjidic edict.txt kanjidic.txt src/ekan

install:
	cp japanese /usr/panini/targets

deepclean: clean

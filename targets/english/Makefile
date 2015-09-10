english: bootstrap
	echo Compiling english.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o english

bootstrap:
	touch $(PANINI)/tools/wiktionary/english 
	cat $(PANINI)/tools/wiktionary/english | sort | uniq > src/bootstrap
	touch bootstrap

clean:
	rm -f english core bootstrap out.txt src/bootstrap k-out

install:
	cp english /usr/panini/targets

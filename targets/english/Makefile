english: bootstrap
	echo Compiling english.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o english

bootstrap:
	cp $(PANINI)/tools/wiktionary/english ./bootstrap
	cat bootstrap | sort | uniq > src/bootstrap

clean:
	rm -f english core bootstrap out.txt src/bootstrap k-out

install:
	cp english /usr/panini/targets

english: bootstrap
	echo Compiling english.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o english

bootstrap:
	./bootstrap.sh > bootstrap
	cat bootstrap | sort | uniq > src/bootstrap

clean:
	rm -f english core src/bootstrap

install:
	cp english /usr/panini/targets

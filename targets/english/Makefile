english: bootstrap.out
	echo Compiling english.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o english

bootstrap.out:
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o english
	$(PANINI)/tools/learn/bootstrap bootstrap.txt
	cat bootstrap.out | sort | uniq > src/bootstrap

clean:
	rm -f english core bootstrap.out src/bootstrap

install:
	cp english /usr/panini/targets

czech: stage1
	echo
	echo Compiling czech.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o czech

stage1: data.txt vocab.out
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo -n Learning Czech
	$(PANINI)/tools/learn/pp data.txt 3
	mv pp.out stage1
	cat stage1 | sort | uniq > src/pp-out

# Something that imports some vocabulary. 
vocab.out: 
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo -n Learning vocab
	$(PANINI)/tools/learn/vocab vocab.txt > vocab.out
	cat vocab.out | sort | uniq > src/vocab

clean:
	rm -f czech core vocab.out pp.txt src/vocab src/pp-out
	rm -f CzeBKR CzeKMS stage1

install:
	cp czech /usr/panini/targets

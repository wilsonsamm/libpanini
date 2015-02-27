czech: vocab.out
	echo Compiling czech.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > czech

# Something that imports some vocabulary. 
vocab.out: 
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo -n Learning vocab
	$(PANINI)/tools/learn/vocab vocab.txt

clean:
	rm -f czech core vocab.out

install:
	cp czech /usr/panini/languages

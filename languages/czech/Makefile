czech: 
	echo Compiling czech.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > czech

# Something that imports some vocabulary. It doesn't work yet, I don't know why
vocab.out:
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo -n Learning vocab
	$(PANINI)/tools/learn/vocab vocab.txt

clean:
	rm -f czech core vocab.out

install:
	cp czech /usr/panini/languages

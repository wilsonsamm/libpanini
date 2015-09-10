czech: k-out
	echo Compiling czech.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o czech

kfile-czech:
	touch $(PANINI)/tools/wiktionary/kfile-czech
	cp $(PANINI)/tools/wiktionary/kfile-czech ./

# Something that imports some vocabulary. 
k-out: kfile-czech
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo Learning vocab
	$(PANINI)/tools/learn/kfile kfile-czech
	touch src/vocab.out
	cat k-out src/vocab.out | sort | uniq > vocab.out
	rm src/vocab.out
	cp vocab.out src/

install:
	cp czech /usr/panini/targets
	
clean:
	rm -rf vocab.out czech kfile-czech

deepclean:
	rm -rf src/vocab.out

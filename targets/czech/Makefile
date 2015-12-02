czech: 
	touch $(PANINI)/tools/wiktionary/czech.panini
	cat $(PANINI)/tools/wiktionary/czech.panini src/wt | sort | uniq > ./wt
	mv ./wt	src/
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o czech

install:
	cp czech /usr/panini/targets
	
clean:
	rm -rf vocab.out czech kfile-czech

deepclean: clean
	rm -rf src/vocab.out

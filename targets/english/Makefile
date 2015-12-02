english: wt
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o english

# Get the new words from the wiktionary and wordnet.
wt:
	touch $(PANINI)/tools/wiktionary/english.panini
	cat $(PANINI)/tools/wiktionary/english.panini src/wt | sort | uniq >> ./wt
#	touch $(PANINI)/tools/wn/english.panini
#	cat $(PANINI)/tools/wn/english.panini src/wt | sort | uniq >> ./wt
	mv ./wt	src/
	touch wt

clean:
	rm -f english core wt

install:
	cp english /usr/panini/targets

deepclean: clean

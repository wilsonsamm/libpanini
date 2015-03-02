czech: vocab.out $(PP_SWORD_CZ)
	echo
	echo Compiling czech.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o czech

# Something that imports some vocabulary. 
vocab.out: 
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo -n Learning vocab
	$(PANINI)/tools/learn/vocab vocab.txt
	cat vocab.out | sort | uniq > src/vocab

# Read the bibles and learn some Czech.
$(PP_SWORD_CZ):
	$(PANINI)/tools/sword/query english $(PP_SWORD_EN) czech $(PP_SWORD_CZ) > pp.txt

clean:
	rm -f czech core vocab.out pp.txt src/vocab

install:
	cp czech /usr/panini/languages

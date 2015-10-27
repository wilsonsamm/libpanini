all: parse-wt testword allwords

parse-wt: parse-wt.o
	$(CCOMPILER) -o parse-wt parse-wt.o -lpanini
parse-wt.o : parse-wt.c
	$(CCOMPILER) $(CCOPTS) parse-wt.c
	
testword: testword.o
	$(CCOMPILER) -o testword testword.o -lpanini
testword.o : testword.c
	$(CCOMPILER) $(CCOPTS) testword.c
	
allwords: allwords.o
	$(CCOMPILER) -o allwords allwords.o -lpanini
allwords.o : allwords.c
	$(CCOMPILER) $(CCOPTS) allwords.c

clean:
	rm -rf english kfile* *.o *.panini parse-wt testword allwords AllWords.txt
	
deepclean: clean
	rm -rf wt-*

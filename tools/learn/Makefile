all: vocab pp bootstrap

pp: pp.o readfile.o
	$(CCOMPILER) -o pp pp.o readfile.o $(PANINI)/libpanini.a

vocab:	vocab.o readfile.o
	$(CCOMPILER) -o vocab vocab.o readfile.o $(PANINI)/libpanini.a

word:	word.o 
	$(CCOMPILER) -o word word.o $(PANINI)/libpanini.a

bootstrap: bootstrap.o readfile.o
	$(CCOMPILER) -o bootstrap bootstrap.o readfile.o $(PANINI)/libpanini.a

pp.o: pp.c readfile.h
	$(CCOMPILER) $(CCOPTS) pp.c

vocab.o: vocab.c readfile.h
	$(CCOMPILER) $(CCOPTS) vocab.c

readfile.o: readfile.c readfile.h
	$(CCOMPILER) $(CCOPTS) readfile.c

word.o: word.c
	$(CCOMPILER) $(CCOPTS) word.c

bootstrap.o: bootstrap.c readfile.h
	$(CCOMPILER) $(CCOPTS) bootstrap.c

clean: 
	rm -rf vocab pp bootstrap core *.o 

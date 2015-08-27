all: vocab pp bootstrap freedict kfile

pp: pp.o readfile.o
	$(CCOMPILER) -o pp pp.o readfile.o $(PANINI)/libpanini.a

vocab:	vocab.o readfile.o
	$(CCOMPILER) -o vocab vocab.o readfile.o $(PANINI)/libpanini.a

kfile:	kfile.o readfile.o
	$(CCOMPILER) -o kfile kfile.o readfile.o $(PANINI)/libpanini.a

freedict:	freedict.o readfile.o
	$(CCOMPILER) -o freedict freedict.o readfile.o $(PANINI)/libpanini.a

word:	word.o 
	$(CCOMPILER) -o word word.o $(PANINI)/libpanini.a

bootstrap: bootstrap.o readfile.o
	$(CCOMPILER) -o bootstrap bootstrap.o readfile.o $(PANINI)/libpanini.a

kfile.o: kfile.c readfile.h
	$(CCOMPILER) $(CCOPTS) kfile.c

pp.o: pp.c readfile.h
	$(CCOMPILER) $(CCOPTS) pp.c

freedict.o: freedict.c readfile.h
	$(CCOMPILER) $(CCOPTS) freedict.c

vocab.o: vocab.c readfile.h
	$(CCOMPILER) $(CCOPTS) vocab.c

readfile.o: readfile.c readfile.h
	$(CCOMPILER) $(CCOPTS) readfile.c

word.o: word.c
	$(CCOMPILER) $(CCOPTS) word.c

bootstrap.o: bootstrap.c readfile.h
	$(CCOMPILER) $(CCOPTS) bootstrap.c

clean: 
	rm -rf vocab pp bootstrap core kfile *.o 

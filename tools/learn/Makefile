all: vocab pp

pp: pp.o readfile.o
	gcc -o pp pp.o readfile.o $(PANINI)/libpanini.a

vocab:	vocab.o readfile.o
	gcc -o vocab vocab.o readfile.o $(PANINI)/libpanini.a

pp.o: pp.c readfile.h
	gcc $(CCOPTS) pp.c

vocab.o: vocab.c readfile.h
	gcc $(CCOPTS) vocab.c

readfile.o: readfile.c readfile.h
	gcc $(CCOPTS) readfile.c
	
clean: 
	rm -rf vocab core *.o



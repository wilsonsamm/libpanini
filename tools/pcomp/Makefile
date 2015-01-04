pcomp: main.o check.o ontology.o definition.o for.o tag.o
	gcc -o pcomp main.o check.o ontology.o definition.o for.o tag.o \
	../../list.o ../../list-tokenise.o

main.o: main.c
	gcc $(CCOPTS) -o main.o main.c

check.o: check.c
	gcc $(CCOPTS) -o check.o check.c

ontology.o: ontology.c
	gcc $(CCOPTS) -o ontology.o ontology.c

definition.o: definition.c
	gcc $(CCOPTS) -o definition.o definition.c

for.o: for.c
	gcc $(CCOPTS) -o for.o for.c

tag.o: tag.c
	gcc $(CCOPTS) -o tag.o tag.c

clean:
	rm -rf pcomp *.o *.gch core 

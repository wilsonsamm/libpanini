all: clean enzh

enzh: enzh.o
	gcc -o enzh enzh.o -lpanini
	
enzh.o: enzh.c
	gcc $(CCOPTS) -g -c -Wall enzh.c
	
clean:
	rm -rf enzh enzh.o *.panini *.txt

deepclean: clean

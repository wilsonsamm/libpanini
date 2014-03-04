all: attest
attest: attest.o
	gcc -o attest attest.o -ltranny
	
attest.o: attest.c
	gcc -g -c -Wall attest.c
	
clean:
	rm -rf attest attest.o

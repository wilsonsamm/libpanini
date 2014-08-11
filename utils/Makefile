all: bilingual interrogator words attest

bilingual: bilingual.o headwords.o strip.o
	gcc -o bilingual bilingual.o headwords.o strip.o -lpanini

interrogator: interrogator.o headwords.o strip.o
	gcc -o interrogator interrogator.o headwords.o strip.o -lpanini

words: words.o headwords.o
	gcc -o words headwords.o words.o -lpanini

attest: attest.o
	gcc -o attest attest.o -lpanini
	
attest.o: attest.c
	gcc -ggdb -Wall -c attest.c

words.o: words.c
	gcc -ggdb -Wall -c words.c

bilingual.o: bilingual.c
	gcc -ggdb -Wall -c bilingual.c

strip.o: strip.c
	gcc -ggdb -Wall -c strip.c

headwords.o: headwords.c
	gcc -ggdb -Wall -c headwords.c	

interrogator.o: interrogator.c
	gcc -ggdb -Wall -c interrogator.c
		
clean:
	rm -rf *.o gmon.out bilingual interrogator words attest core


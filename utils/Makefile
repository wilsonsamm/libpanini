all: bilingual interrogator

bilingual: bilingual.o headwords.o strip.o
	gcc -o bilingual bilingual.o headwords.o strip.o -ltranny

interrogator: interrogator.o headwords.o strip.o
	gcc -o interrogator interrogator.o headwords.o strip.o -ltranny

bilingual.o: bilingual.c
	gcc -ggdb -Wall -c bilingual.c

strip.o: strip.c
	gcc -ggdb -Wall -c strip.c

headwords.o: headwords.c
	gcc -ggdb -Wall -c headwords.c	

interrogator.o: interrogator.c
	gcc -ggdb -Wall -c interrogator.c
		
clean:
	rm -rf *.o bilingual interrogator


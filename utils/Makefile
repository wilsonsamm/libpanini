bilingual: bilingual.o headwords.o strip.o
	gcc -o bilingual bilingual.o headwords.o strip.o -ltranny

bilingual.o: bilingual.c
	gcc -ggdb -Wall -c bilingual.c

strip.o: strip.c
	gcc -ggdb -Wall -c strip.c

headwords.o: headwords.c
	gcc -ggdb -Wall -c headwords.c	
	
clean:
	rm -rf *.o bilingual


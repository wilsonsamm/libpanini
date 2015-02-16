flashcards: main.o genders.o vocab.o
	gcc -o flashcards main.o genders.o vocab.o -lpanini

main.o: main.c
	gcc -g -c -Wall main.c

vocab.o: vocab.c
	gcc -g -c -Wall vocab.c

genders.o: genders.c
	gcc -g -c -Wall genders.c

clean:
	rm -rf flashcards *.o genders.csv vocab.csv

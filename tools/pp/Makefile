pp:	main.o 
	gcc -o pp main.o $(PANINI)/libpanini.a

main.o: main.c
	gcc $(CCOPTS) main.c
	
clean: 
	rm -rf pp core *.o



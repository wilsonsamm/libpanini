pp:	main.o
	gcc -o pp main.o -lpanini

main.o: main.c
	gcc -g -c -Wall main.c

langclean:
	rm -rf spanish-pp-out
	rm -rf spanish/pp-out

clean: langclean
	rm -rf pp *.o

langs: pp langclean
	./run.sh

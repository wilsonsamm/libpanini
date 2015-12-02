all: dict-data thes-data

thes-data: thes-data.o
	gcc -o thes-data thes-data.o -lpanini
	
thes-data.o: thes-data.c
	gcc -g -c -Wall thes-data.c
	
dict-data: dict-data.o
	gcc -o dict-data dict-data.o -lpanini
	
dict-data.o: dict-data.c
	gcc -g -c -Wall dict-data.c
	
clean:
	rm -rf thes-data dict-data *.o *.thes *.dict *.aux *.tex *.log

deepclean: clean
	rm -rf *.pdf

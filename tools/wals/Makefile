
all: parselang language.csv
	./makelangs.sh

parselang: parselang.o
	$(CCOMPILER) -o parselang parselang.o $(PANINI)/libpanini.a
	
parselang.o: parselang.c
	$(CCOMPILER) $(CCOPTS) parselang.c

language.csv:
	wget http://wals.info/static/download/wals-language.csv.zip
	unzip wals-language.csv.zip 

clean:
	rm -rf wals-language.csv.zip README.txt
	rm -rf *.o parselang 
	rm -rf ain cze sno wel prh

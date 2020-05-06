all: fonctions_lucas.o utils.o
	gcc -I ../lib/libparser -I ../lib/librequest/api  -L ../lib/libparser -L ../lib/librequest -o serverTest serverTest.c fonctions_lucas.o -lparser -lrequest
	echo "Faut faire ça : 'export LD_LIBRARY_PATH=.'"

fonctions_lucas.o:
	gcc -c ../src/fonctions_lucas.c -Wall

utils.o:
	gcc -c ../src/utils.c -Wall

clean:
	rm -rf *.o
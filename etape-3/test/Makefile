all:	
	gcc -I ../lib/libparser -I ../lib/librequest/api  -L ../lib/libparser -L ../lib/librequest -o serverTest serverTest.c fonctions_lucas.o -lparser -lrequest
	echo "Faut faire ça : 'export LD_LIBRARY_PATH=.'"

fonctions_lucas.o:
	gcc -c fonctions_lucas.c -Wall


all: connection.o response.o normalization.o fonctions_lucas.o fonctions_marin.o utils.o
	gcc -I ../lib/libparser -I ../lib/librequest/api -I ../src -L ../lib/libparser -L ../lib/librequest -o serverTest serverTest.c connection.o response.o normalization.o fonctions_lucas.o fonctions_marin.o utils.o -lparser -lrequest -lmagic
	echo "Faut faire ça : 'export LD_LIBRARY_PATH=.'"

connection.o: ../src/connection.c ../src/connection.h ../src/utils.h
	gcc -c ../src/connection.c -Wall

response.o: ../src/response.c ../src/connection.h ../src/utils.h
	gcc -I ../lib/librequest/api -c ../src/response.c -Wall

normalization.o: ../src/normalization.c ../src/normalization.h ../src/utils.h
	gcc -c ../src/normalization.c -Wall

fonctions_lucas.o: ../src/fonctions_lucas.c ../src/fonctions_lucas.h ../src/utils.h ../src/normalization.h
	gcc -c ../src/fonctions_lucas.c -Wall

fonctions_marin.o: ../src/fonctions_marin.c ../src/fonctions_marin.h ../src/utils.h
	gcc -I ../lib/libparser -c ../src/fonctions_marin.c -Wall

utils.o: ../src/utils.c ../src/utils.h
	gcc -c ../src/utils.c -Wall

clean:
	rm -rf *.o

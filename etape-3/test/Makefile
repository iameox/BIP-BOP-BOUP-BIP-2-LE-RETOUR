all: response.o normalization.o fonctions_lucas.o utils.o 
	gcc -I ../lib/libparser -I ../lib/librequest/api -I ../src -L ../lib/libparser -L ../lib/librequest -o serverTest serverTest.c response.o normalization.o fonctions_lucas.o utils.o -lparser -lrequest
	echo "Faut faire ça : 'export LD_LIBRARY_PATH=.'"

response.o: ../src/response.c ../src/response.h
	gcc -I ../lib/librequest/api -c ../src/response.c -Wall

normalization.o: ../src/normalization.c ../src/utils.h
	gcc -c ../src/normalization.c -Wall

fonctions_lucas.o: ../src/fonctions_lucas.c ../src/utils.h
	gcc -c ../src/fonctions_lucas.c -Wall

utils.o: ../src/utils.c ../src/utils.h
	gcc -c ../src/utils.c -Wall

clean:
	rm -rf *.o ../src/*.o

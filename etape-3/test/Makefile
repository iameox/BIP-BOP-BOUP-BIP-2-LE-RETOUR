all: connection.o response.o normalization.o ressource.o semantic.o utils.o
	gcc -I ../lib/libparser -I ../lib/librequest/api -I ../src -L ../lib/libparser -L ../lib/librequest -o serverTest serverTest.c connection.o response.o normalization.o ressource.o semantic.o utils.o -lparser -lrequest -lmagic
	echo "Faut faire ça : 'export LD_LIBRARY_PATH=.'"

connection.o: ../src/connection.c ../src/connection.h ../src/utils.h
	gcc -c ../src/connection.c -Wall

response.o: ../src/response.c ../src/connection.h ../src/utils.h
	gcc -I ../lib/librequest/api -c ../src/response.c -Wall

normalization.o: ../src/normalization.c ../src/normalization.h ../src/utils.h
	gcc -c ../src/normalization.c -Wall

ressource.o: ../src/ressource.c ../src/ressource.h ../src/utils.h ../src/normalization.h
	gcc -c ../src/ressource.c -Wall

semantic.o: ../src/semantic.c ../src/semantic.h ../src/utils.h
	gcc -I ../lib/libparser -c ../src/semantic.c -Wall

utils.o: ../src/utils.c ../src/utils.h
	gcc -c ../src/utils.c -Wall

clean:
	rm -rf *.o

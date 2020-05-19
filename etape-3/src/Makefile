all: main.o fonctions_marin.o utils.o
	gcc -I ../lib/libparser -I ../lib/librequest/api -I ../src -L ../lib/libparser -L ../lib/librequest -o fonctions_marin main.c fonctions_marin.o utils.o -lparser -lrequest
	echo "Faut faire ça : 'export LD_LIBRARY_PATH=.'"

main.o: ../src/main.c ../src/fonctions_marin.h ../src/utils.h
	gcc -c ../src/main.c -Wall

fonctions_marin.o: ../src/fonctions_marin.c ../src/fonctions_marin.h
	gcc -c ../src/fonctions_marin.c -Wall

utils.o: ../src/utils.c ../src/utils.h
	gcc -c ../src/utils.c -Wall

clean:
	rm -rf *.o ../src/*.o

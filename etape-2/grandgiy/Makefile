all: main.o api.o abnf.o http.o structures.o functions.o
	gcc -o parser main.o api.o abnf.o http.o structures.o functions.o

clean:
	rm -rf *.o


main.o: main.c api.h
	gcc -c main.c -Wall

api.o: api.c api.h src/*.h
	gcc -c api.c -Wall

abnf.o: src/abnf.c src/*.h
	gcc -c src/abnf.c -Wall

http.o: src/http.c src/*.h
	gcc -c src/http.c -Wall

structures.o: src/structures.c src/*.h
	gcc -c src/structures.c -Wall

functions.o: src/functions.c src/*.h
	gcc -c src/functions.c -Wall
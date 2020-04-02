all: test.o abnf.o http.o structures.o functions.o
	gcc -o test test.o abnf.o http.o structures.o functions.o

test.o: test.c http.h
	gcc -c test.c -Wall

abnf.o: abnf.c abnf.h structures.h functions.h
	gcc -c abnf.c -Wall

http.o: http.c http.h abnf.h structures.h functions.h
	gcc -c http.c -Wall

structures.o: structures.c functions.h
	gcc -c structures.c -Wall

functions.o: functions.c functions.h
	gcc -c functions.c -Wall

clean:
	rm -rf *.o
all: main.o api.o parser.o abnf.o functions.o
	gcc -o main main.o api.o parser.o abnf.o functions.o

main.o: main.c api.h
	gcc -c main.c -Wall

api.o: api.c *.h
	gcc -c api.c -Wall

parser.o: parser.c *.h
	gcc -c parser.c -Wall

abnf.o: abnf.c abnf.h
	gcc -c abnf.c -Wall

functions.o: functions.c functions.h
	gcc -c functions.c -Wall

clean:
	rm -rf *.o
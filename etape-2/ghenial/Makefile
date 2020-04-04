all: parser.o abnf.o functions.o
	gcc -o agaga parser.o abnf.o functions.o

parser.o: parser.c *.h
	gcc -c parser.c -Wall

abnf.o: abnf.c *.h
	gcc -c abnf.c -Wall

functions.o: functions.c *.h
	gcc -c functions.c -Wall

clean:
	rm -rf *.o
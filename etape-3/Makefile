all: response.o resource.o semantic.o utils.o
	gcc -I lib/libparser -I lib/librequest/api -I src -L lib/libparser -L lib/librequest -o server server.c response.o resource.o semantic.o utils.o -lparser -lrequest -lmagic

response.o: src/response.c src/response.h src/utils.h
	gcc -I lib/librequest/api -c src/response.c -Wall

resource.o: src/resource.c src/resource.h src/utils.h
	gcc -c src/resource.c -Wall

semantic.o: src/semantic.c src/semantic.h src/utils.h
	gcc -I lib/libparser -c src/semantic.c -Wall

utils.o: src/utils.c src/utils.h
	gcc -c src/utils.c -Wall

clean:
	rm -rf *.o

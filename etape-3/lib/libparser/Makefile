httpparser: main.o libparser.so 
	gcc $(CCFLAGS) -o $@ $< -L. -lparser
	echo "Faut faire ça : 'export LD_LIBRARY_PATH=.'"

main.o: main.c 
	gcc $(CCFLAGS) -c $< -o $@

.PRECIOUS: %.c %.h

all: httpparser

clean : 
	rm -rf httpparser
	rm -rf *.o 



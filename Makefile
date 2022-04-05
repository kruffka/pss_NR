.PHONY: clean

all: main.o
	gcc -o pss main.o

main.o:
	gcc -c main.c

clean:
	rm -rf *.o pss
.PHONY: clean

SOURCE = main.c pss_nr.c tools.c cdot_prod.c
FLAGS = -lm

all: pss

pss: $(SOURCE)
	gcc $(SOURCE) $(FLAGS) -o pss

clean:
	rm -rf *.o pss
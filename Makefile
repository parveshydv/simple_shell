.phony all:
all: ssi

main: ssi.c
	gcc ssi.c -o ssi -lreadline

.PHONY clean:
clean:
	-rm -rf *.o *.exe

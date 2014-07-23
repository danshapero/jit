.PHONY: clean

CC = gcc
CXX = g++

LFLAGS = -ltcc -ldl

%.o: %.c
	$(CC) -c $< $(IFLAGS)

tcc_jit: tcc_jit.o
	$(CC) -o tcc_jit tcc_jit.o $(LFLAGS)

clean:
	rm -rf *.o *~ tcc_jit


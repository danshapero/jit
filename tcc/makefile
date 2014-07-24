.PHONY: clean

CC = clang
CXX = clang++
CFLAGS = -std=c99
CXXflags = -std=c++11

%.o: %.c
	$(CC) -c $< $(IFLAGS) $(CFLAGS)

tcc_jit: tcc_jit.o
	$(CC) -o tcc_jit tcc_jit.o -ltcc -ldl

linalg_example: stack.o graph.o sparse_matrix.o jit_sparse_matrix_kernels.o linalg_example.o
	$(CC) -o $@ $^ $(CFLAGS) -lm -ltcc -ldl

clean:
	rm -rf *.o *~ tcc_jit linalg_example


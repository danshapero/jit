.PHONY: clean

CC = clang
CXX = clang++
CFLAGS = -std=c99
CXXflags = -std=c++11
IFLAGS = -I../include
LFLAGS = -lm -ltcc -ldl

all: linalg_example linalg_bcsr_example linalg_profile_jit

%.o: %.c
	$(CC) -c $< $(IFLAGS) $(CFLAGS)

tcc_jit: tcc_jit.o
	$(CC) -o tcc_jit tcc_jit.o $(LFLAGS)

linalg_example: stack.o graph.o sparse_matrix.o jit_sparse_matrix_kernels.o linalg_example.o
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

linalg_bcsr_example: stack.o graph.o sparse_matrix.o block_sparse_matrix.o jit_block_sparse_matrix_kernels.o linalg_bcsr_example.o
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

linalg_profile_jit: stack.o graph.o sparse_matrix.o block_sparse_matrix.o jit_sparse_matrix_kernels.o jit_block_sparse_matrix_kernels.o linalg_profile_jit.o
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

clean:
	rm -rf *.o *~ tcc_jit linalg_example linalg_bcsr_example linalg_profile_jit


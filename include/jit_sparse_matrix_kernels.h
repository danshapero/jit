
#include "libtcc.h"
#include "sparse_matrix.h"

#ifndef JIT_SPARSE_MATRIX_KERNELS_H
#define JIT_SPARSE_MATRIX_KERNELS_H

void jitCompileMatvec(TCCState *s, char *name);
Matvec jitGetMatvec(TCCState *s, char *name);

#endif

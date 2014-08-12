
#include "libtcc.h"
#include "sparse_matrix.h"

#ifndef JIT_SPARSE_MATRIX_KERNELS_H
#define JIT_SPARSE_MATRIX_KERNELS_H

Matvec jitCompileMatvec(TCCState *s);

#endif

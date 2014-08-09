
#include "libtcc.h"
#include "block_sparse_matrix.h"

#ifndef JIT_BLOCK_SPARSE_MATRIX_KERNELS_H
#define JIT_BLOCK_SPARSE_MATRIX_KERNELS_H

BlockMatvec jitCompileBlockMatvec(TCCState *s, int mc, int nc);
BlockMatvec jitCompileSpecializedBlockMatvec(TCCState *s, int mc, int nc);
BlockMatvec jitCompileUnrolledBlockMatvec(TCCState *s, int mc, int nc);

#endif

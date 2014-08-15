
#include "libtcc.h"
#include "block_sparse_matrix.h"

#ifndef JIT_BLOCK_SPARSE_MATRIX_KERNELS_H
#define JIT_BLOCK_SPARSE_MATRIX_KERNELS_H

void jitCompileBlockMatvec(TCCState *s, char *name, int mc, int nc);
void jitCompileSpecializedBlockMatvec(TCCState *s, char *name, int mc, int nc);
void jitCompileUnrolledBlockMatvec(TCCState *s, char *name, int mc, int nc);
BlockMatvec jitGetBlockMatvec(TCCState *s, char *name);

#endif

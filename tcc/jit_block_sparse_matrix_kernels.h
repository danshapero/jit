
#include "libtcc.h"
#include "block_sparse_matrix.h"

BlockMatvec jitCompileBlockMatvec(TCCState *s, int mc, int nc);
BlockMatvec jitCompileSpecializedBlockMatvec(TCCState *s, int mc, int nc);

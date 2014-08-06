
#include "block_sparse_matrix.h"

BlockMatvec jitCompileBlockMatvec(int mc, int nc);
BlockMatvec jitCompileSpecializedBlockMatvec(int mc, int nc);

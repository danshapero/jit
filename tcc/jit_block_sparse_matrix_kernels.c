
#include <stdlib.h>

#include "block_sparse_matrix.h"
#include "libtcc.h"

BlockMatvec jitCompileBlockMatvec(int mc, int nc) {

    char block_matvec_code[] =
    "#include \"block_sparse_matrix.h\"                                 \n"
    "void jit_bcsr_matvec(BlockSparseMatrix *A, double *x, double *y) { \n"
    "    int I, J, K, i, j, k, l, M, N, index;                          \n"
    "    double z;                                                      \n"
    "                                                                   \n"
    "    for (i = 0; i < A->m; i++) y[i] = 0.0;                         \n"
    "                                                                   \n"
    "    M = A->m / A->mc;                                              \n"
    "    for (I = 0; I < M; I++) {                                      \n"
    "        for (K = A->ptr[I]; K < A->ptr[I + 1]; K++) {              \n"
    "            J = A->node[K];                                        \n"
    "                                                                   \n"
    "            for (k = 0; k < A->mc; k++) {                          \n"
    "                i = A->mc * I + k;                                 \n"
    "                for (l = 0; l < A->nc; l++) {                      \n"
    "                    j = A->nc * J + l;                             \n"
    "                    index = (K * A->mc + k) * A->nc + l;           \n"
    "                                                                   \n"
    "                    y[i] += A->val[index] * x[j];                  \n"
    "                }                                                  \n"
    "            }                                                      \n"
    "        }                                                          \n"
    "    }                                                              \n"
    "}                                                                  \n";

    TCCState *s;
    BlockMatvec jit_compiled_block_matvec;

    /* Make a new TCC context and set the output of the compilation to be a
       location in memory, rather than an object file */
    s = tcc_new();
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    /* Compile the matvec code defined above */
    tcc_compile_string(s, block_matvec_code);

    /* Relocate the object code to a new, executable location in memory */
    if (tcc_relocate(s) < 0) return NULL;

    /* Get a function pointed to the location in memory of the object code */
    jit_compiled_block_matvec = tcc_get_symbol(s, "jit_bcsr_matvec");

    /* Now that we have the function, we can delete the TCC context */
    tcc_delete(s);

    return jit_compiled_block_matvec;

}

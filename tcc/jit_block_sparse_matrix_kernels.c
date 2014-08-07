
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "block_sparse_matrix.h"
#include "libtcc.h"

BlockMatvec jitCompileBlockMatvec(TCCState *s, int mc, int nc) {

    char block_matvec_code[] =
    "#include \"block_sparse_matrix.h\"                                 \n"
    "void jit_bcsr_matvec(BlockSparseMatrix *A, double *x, double *y) { \n"
    "    int I, J, K, i, j, M, N, index;                                \n"
    "    double z;                                                      \n"
    "                                                                   \n"
    "    for (i = 0; i < A->m; i++) y[i] = 0.0;                         \n"
    "                                                                   \n"
    "    M = A->m / A->mc;                                              \n"
    "    for (I = 0; I < M; I++) {                                      \n"
    "        for (K = A->ptr[I]; K < A->ptr[I + 1]; K++) {              \n"
    "            J = A->node[K];                                        \n"
    "                                                                   \n"
    "            index = K * A->mc * A->nc;                             \n"
    "            for (i = A->mc * I; i < A->mc * (I + 1); i++) {        \n"
    "                for (j = A->nc * J; j < A->nc * (J + 1); j++) {    \n"
    "                    y[i] += A->val[index] * x[j];                  \n"
    "                    index++;                                       \n"
    "                }                                                  \n"
    "            }                                                      \n"
    "        }                                                          \n"
    "    }                                                              \n"
    "}                                                                  \n";

    BlockMatvec jit_compiled_block_matvec;

    /* Set the output of the compilation to be a location in memory,
       rather than an object file */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    /* Compile the matvec code defined above */
    tcc_compile_string(s, block_matvec_code);

    /* Relocate the object code to a new, executable location in memory */
    if (tcc_relocate(s) < 0) return NULL;

    /* Get a function pointed to the location in memory of the object code */
    jit_compiled_block_matvec = tcc_get_symbol(s, "jit_bcsr_matvec");

    return jit_compiled_block_matvec;

}


BlockMatvec jitCompileSpecializedBlockMatvec(TCCState *s, int mc, int nc) {

    char block_matvec_code[24 * 70];
    char line[70];

    strcpy(block_matvec_code, 
    "#include \"block_sparse_matrix.h\"                                 \n"
    "void jit_sbcsr_matvec(BlockSparseMatrix *A, double *x, double *y) {\n"
    "    int I, J, K, i, j, M, N, index;                                \n"
    "    double z;                                                      \n"
    "                                                                   \n"
    "    for (i = 0; i < A->m; i++) y[i] = 0.0;                         \n"
    "                                                                   \n"
    );

    sprintf(line,
    "    M = A->m / %d;                                                 \n",
    mc);
    strcat(block_matvec_code, line);

    strcat(block_matvec_code,
    "    for (I = 0; I < M; I++) {                                      \n"
    "        for (K = A->ptr[I]; K < A->ptr[I + 1]; K++) {              \n"
    "            J = A->node[K];                                        \n"
    "                                                                   \n"
    );


    sprintf(line,
    "            index = K * %d * %d;                                   \n",
    mc, nc);
    strcat(block_matvec_code, line);

    sprintf(line,
    "            for (i = %d * I; i < %d * (I + 1); i++) {              \n",
    mc, mc);
    strcat(block_matvec_code, line);

    sprintf(line,
    "                for (j = %d * J; j < %d * (J + 1); j++) {          \n",
    nc, nc);
    strcat(block_matvec_code, line);

    strcat(block_matvec_code,
    "                    y[i] += A->val[index] * x[j];                  \n"
    "                    index++;                                       \n"
    "                }                                                  \n"
    "            }                                                      \n"
    "        }                                                          \n"
    "    }                                                              \n"
    "}                                                                  \n"
    );


    BlockMatvec jit_compiled_block_matvec;

    /* Set the output of the compilation to be a location in memory,
       rather than an object file */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    /* Compile the matvec code defined above */
    tcc_compile_string(s, block_matvec_code);

    /* Relocate the object code to a new, executable location in memory */
    if (tcc_relocate(s) < 0) return NULL;

    /* Get a function pointed to the location in memory of the object code */
    jit_compiled_block_matvec = tcc_get_symbol(s, "jit_sbcsr_matvec");

    return jit_compiled_block_matvec;
}


BlockMatvec jitCompileUnrolledBlockMatvec(TCCState *s, int mc, int nc) {

    char block_matvec_code[100 * 70];
    //TODO make this depend on how many lines of code we expect to need

    char line[70];
    int k, l;

    strcpy(block_matvec_code, 
    "#include \"block_sparse_matrix.h\"                                 \n"
    "void jit_ubcsr_matvec(BlockSparseMatrix *A, double *x, double *y) {\n"
    "    int I, J, K, i, j, M, N, index;                                \n"
    "    double z;                                                      \n"
    "                                                                   \n"
    "    for (i = 0; i < A->m; i++) y[i] = 0.0;                         \n"
    "                                                                   \n"
    );

    sprintf(line,
    "    M = A->m / %d;                                                 \n",
    mc);
    strcat(block_matvec_code, line);

    strcat(block_matvec_code, 
    "    for (I = 0; I < M; I++) {                                      \n"
    "        for (K = A->ptr[I]; K < A->ptr[I + 1]; K++) {              \n"
    "            J = A->node[K];                                        \n"
    );


    sprintf(line, 
    "            index = K * %d * %d;                                   \n",
    mc, nc);
    strcat(block_matvec_code, line);

    sprintf(line, 
    "            i = %d * I;                                            \n",
    mc);
    strcat(block_matvec_code, line);

    for (k = 0; k < mc; k++) {
        sprintf(line,
    "            j = %d * J;                                            \n",
        nc);
	strcat(block_matvec_code, line);

        for (l = 0; l < nc; l++) {
            strcat(block_matvec_code, 
    "            y[i] += A->val[index] * x[j];  index++;  j++;          \n"
            );
        }
	strcat(block_matvec_code,
    "            i++;                                                   \n"
        );
    }

    strcat(block_matvec_code,
    "        }                                                          \n"
    "    }                                                              \n"
    "}                                                                  \n");

//    printf(block_matvec_code);

    BlockMatvec jit_compiled_block_matvec;

    /* Set the output of the compilation to be a location in memory,
       rather than an object file */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    /* Compile the matvec code defined above */
    tcc_compile_string(s, block_matvec_code);

    /* Relocate the object code to a new, executable location in memory */
    if (tcc_relocate(s) < 0) return NULL;

    /* Get a function pointed to the location in memory of the object code */
    jit_compiled_block_matvec = tcc_get_symbol(s, "jit_ubcsr_matvec");

    return jit_compiled_block_matvec;
}

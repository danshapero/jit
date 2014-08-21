
#include "libtcc.h"
#include "sparse_matrix.h"

#include <string.h>
#include <stdlib.h>


void jitCompileMatvec(TCCState *s, char *name) {
    char matvec_code[30 * 70];

    strcpy(matvec_code, "#include \"../include/sparse_matrix.h\"            \n");
    strcat(matvec_code, "void ");
    strcat(matvec_code, name);
    strcat(matvec_code, "(SparseMatrix *A, double *x, double *y) {          \n"
    "    int m = A->m;                                                      \n"
    "    int *ptr = A->ptr, *node = A->node;                                \n"
    "    double *val = A->val;                                              \n"
    "    int i, j, k;                                                       \n"
    "    double z;                                                          \n"
    "                                                                       \n"
    "    for (i = 0; i < m; i++) {                                          \n"
    "        z = 0.0;                                                       \n"
    "                                                                       \n"
    "        for (k = ptr[i]; k < ptr[i + 1]; k++) {                        \n"
    "            j = node[k];                                               \n"
    "            z += val[k] * x[j];                                        \n"
    "        }                                                              \n"
    "                                                                       \n"
    "        y[i] = z;                                                      \n"
    "    }                                                                  \n"
    "}                                                                      \n");

    Matvec jit_compiled_matvec;

    /* Make a new TCC context and set the output of the compilation to be a
       location in memory, rather than an object file */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    /* Compile the matvec code defined above */
    tcc_compile_string(s, matvec_code);
}


Matvec jitGetMatvec(TCCState *s, char *name) {
    Matvec jit_matvec = tcc_get_symbol(s, name);
    return jit_matvec;
}

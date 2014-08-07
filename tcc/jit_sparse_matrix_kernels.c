
#include "libtcc.h"
#include "sparse_matrix.h"

#include <stdlib.h>


Matvec jitCompileMatvec(TCCState *s) {
    char matvec_code[] = 
    "#include \"sparse_matrix.h\"                               \n"
    "void jit_matvec(SparseMatrix *A, double *x, double *y) {   \n"
    "    int i, j, k;                                           \n"
    "    double z;                                              \n"
    "                                                           \n"
    "    for (i = 0; i < A->m; i++) {                           \n"
    "        z = 0.0;                                           \n"
    "                                                           \n"
    "        for (k = A->ptr[i]; k < A->ptr[i + 1]; k++) {      \n"
    "            j = A->node[k];                                \n"
    "            z += A->val[k] * x[j];                         \n"
    "        }                                                  \n"
    "                                                           \n"
    "        y[i] = z;                                          \n"
    "    }                                                      \n"
    "}                                                          \n";

    Matvec jit_compiled_matvec;

    /* Make a new TCC context and set the output of the compilation to be a
       location in memory, rather than an object file */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    /* Compile the matvec code defined above */
    tcc_compile_string(s, matvec_code);

    /* Relocate the object code to a new, executable location in memory */
    if (tcc_relocate(s) < 0) return NULL;

    /* Get a function pointed to the location in memory of the object code */
    jit_compiled_matvec = tcc_get_symbol(s, "jit_matvec");

    return jit_compiled_matvec;

}

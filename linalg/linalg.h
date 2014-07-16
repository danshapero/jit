#include <jit/jit.h>
#include <math.h>

typedef double (*dot_product) (int n, double x[n], double y[n]);

double native_dot_product(int n, double x[n], double y[n]);

jit_function_t build_dot_product(jit_context_t context);
jit_function_t build_unrolled_dot_product(jit_context_t context, unsigned int r);

#include <linalg.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int i;
    double x[10], y[10];

    dot_product dp = NULL;

    // Evaluate the dot product of x, y using a native function
    dp = native_dot_product;

    for (i = 0; i < 10; i++) {
        x[i] = 1.0;
        y[i] = -1.0;
    }

    double z = dp(10, x, y);
    printf("%g\n", z);


    jit_context_t context = jit_context_create();


    // Now evaluate the dot product using a JIT function
    jit_function_t jitdp = build_dot_product(context);
    dot_product jit_dot_product = jit_function_to_closure(jitdp);

    dp = jit_dot_product;
    z = dp(10, x, y);
    printf("%g\n", z);


    // Now evaluate the dot product using an unrolled JIT function
    jit_function_t u_jitdp = build_unrolled_dot_product(context, 2);
    dot_product u_jit_dot_product = jit_function_to_closure(u_jitdp);

    dp = u_jit_dot_product;
    z = dp(10, x, y);
    printf("%g\n", z);

    jit_context_destroy(context);

    return 0;
}

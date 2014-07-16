#include <linalg.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int i;
    double x[10], y[10];

    dot_product dp = NULL;
    dp = native_dot_product;

    for (i = 0; i < 10; i++) {
        x[i] = 1.0;
        y[i] = -1.0;
    }

    double z = dp(10, x, y);
    printf("%g\n", z);


    jit_context_t context = jit_context_create();
    jit_function_t jitdp = build_dot_product(context);

    dot_product jdp = jit_function_to_closure(jitdp);
    z = jdp(10, x, y);
    printf("%g\n", z);

    jit_context_destroy(context);

    return 0;
}

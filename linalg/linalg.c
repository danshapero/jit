#include <linalg.h>

double native_dot_product(int n, double x[n], double y[n]) {
    int i;
    double dot = 0.0;

    for (i = 0; i < n; i++) {
        dot = dot + x[i] * y[i];
    }

    return dot;
}


jit_function_t build_dot_product(jit_context_t context) {
    jit_context_build_start(context);

    // Create function signature
    jit_type_t params[] = { jit_type_nint,
                            jit_type_void_ptr,
                            jit_type_void_ptr };

    jit_type_t signature = jit_type_create_signature(
        jit_abi_cdecl, jit_type_float64, params, 3, 1
    );

    // Create function to return
    jit_function_t F = jit_function_create(context, signature);
    jit_type_free(signature);

    // n, x, y are function parameters
    jit_value_t n, x, y;
    n = jit_value_get_param(F, 0);
    x = jit_value_get_param(F, 1);
    y = jit_value_get_param(F, 2);

    // i is a temporary int, z is a temporary double
    jit_value_t i, z, xi, yi, tmp1, tmp2;
    i = jit_value_create(F, jit_type_int);
    z = jit_value_create(F, jit_type_float64);
    xi = jit_value_create(F, jit_type_float64);
    yi = jit_value_create(F, jit_type_float64);
    tmp1 = jit_value_create(F, jit_type_float64);
    tmp2 = jit_value_create(F, jit_type_float64);

    // Set i, z to 0
    jit_value_t fconst0 = jit_value_create_float64_constant(F, 
                                                        jit_type_float64, 0.0);
    jit_insn_store(F, z, fconst0);
    jit_value_t iconst0 = jit_value_create_nint_constant(F, jit_type_nint, 0);
    jit_insn_store(F, i, iconst0);
    jit_value_t one = jit_value_create_nint_constant(F, jit_type_nint, 1);

    // Build the loop
    jit_label_t label_for = jit_label_undefined;
    jit_label_t label_after_for = jit_label_undefined;

    jit_insn_label(F, &label_for);
    jit_value_t cmp_i_n = jit_insn_eq(F, i, n);
    jit_insn_branch_if(F, cmp_i_n, &label_after_for);

    // Retrieve x[i] and y[i]
    xi = jit_insn_load_elem(F, x, i, jit_type_float64);
    yi = jit_insn_load_elem(F, y, i, jit_type_float64);

    // Add xi * yi to z
    tmp1 = jit_insn_mul(F, xi, yi);
    tmp2 = jit_insn_add(F, z, tmp1);
    jit_insn_store(F, z, tmp2);

    // Increment i
    i = jit_insn_add(F, i, one);

    // Branch
    jit_insn_branch(F, &label_for);
    jit_insn_label(F, &label_after_for);

    // Return z
    jit_insn_return(F, z);

    jit_context_build_end(context);
    return F;
}

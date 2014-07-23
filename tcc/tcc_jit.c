/* Program demonstrating the use of libtcc for JIT compilation                */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libtcc.h"

int add(int a, int b) { return a + b; }

char my_program[] =
"int fib(int n) {\n"
"    if (n <= 2) return 1;\n"
"    else return fib(n-1) + fib(n-2);\n"
"}\n"
"int foobar(int n) {\n"
"    printf(\"fib(%d) = %d\\n\", n, fib(n));\n"
"    printf(\"add(%d, %d) = %d\\n\", n, 2 * n, add(n, 2 * n));\n"
"    return 1337;\n"
"}\n";

int main(int argc, char **argv)
{
    TCCState *s;
    int (*func)(int);

    s = tcc_new();
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
    tcc_compile_string(s, my_program);
    tcc_add_symbol(s, "add", add);

    /* Relocate the code */
    if (tcc_relocate(s) < 0) return 1;

    func = tcc_get_symbol(s, "foobar");

    tcc_delete(s);

    printf("foobar returned: %d\n", func(32));

    return 0;
}

from pycparser import c_ast, c_parser


class JitSyntaxError(Exception):
    def __init__(self, message):
        self.message = message



'''class ForLoopVisitor(c_ast.NodeVisitor):
    def visit_ForLoop(self, node):
        


class FuncDefVisitor(c_ast.NodeVisitor):
    def visit_FuncDef(self, node):'''



def extract_jit_function_blocks(text):

    jit_start = text.find("@JIT")
    jit_function_blocks = []

    while(jit_start != -1):
        func_body_start = text.find("{{", jit_start)
        func_body_end   = text.find("}}", jit_start)

        if func_body_start == -1 or func_body_end == -1:
            raise JitSyntaxError(
            "JIT invocation at position {0} lacks delimiters {{, }} to denote"
            "beginning and end of JIT function block.".format(jit_start)
            )

        jit_func_header = text[jit_start : func_body_start]
        jit_func_body   = text[func_body_start + 2 : func_body_end]
        jit_function_blocks.append( (jit_func_header, jit_func_body) )

        jit_start = text.find("@JIT", func_body_end + 2)


    return jit_function_blocks


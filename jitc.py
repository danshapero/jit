from pycparser import c_ast, c_parser


class JitFunction(object):
    def __init__(self, args = None, body = None):
        self.args = args
        self.body = body



class JitSyntaxError(Exception):
    def __init__(self, message):
        self.message = message



def extract_jit_functions(text):

    jit_start = text.find("@JIT")
    jit_functions = []

    while(jit_start != -1):
        # Find where the C function to be JIT compiled begins and ends. This
        # is denoted by double curly braces. Better hope those don't appear
        # in comments somewhere or we're really screwed.
        body_start = text.find("{{", jit_start)
        body_end   = text.find("}}", jit_start)

        if body_start == -1 or body_end == -1:
            raise JitSyntaxError(
            "JIT invocation at position {0} lacks delimiters {{, }} to denote"
            "beginning and end of JIT function block.".format(jit_start)
            )


        # Find what the arguments are going to be to the JIT compilation 
        # function and their types.
        args_start = text.find("(", jit_start)
        args_end   = text.find(")", jit_start)

        func_header = {}
        if args_start != -1 and args_end < body_start:
            args_list = text[args_start + 1, args_end].split(',')
            for arg_pair in args_list:
                func_header.insert( arg_pair.split() )


        # Make a JitFunction object encapsulating the arguments and content
        # of the JIT compiled C function.
        func_header = text[jit_start : body_start]
        func_body   = text[body_start + 2 : body_end]
        jit_functions.append( 
                            JitFunction(args = func_header,
                                        body = func_body)
                            )


        # Find the next JIT function
        jit_start = text.find("@JIT", body_end + 2)


    return jit_functions

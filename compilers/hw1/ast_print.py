import compiler
from compiler.ast import *
import sys

def ast_print_py(n, f):

    # Just print the whole program
    if isinstance(n, Module):
        ast_print_py(n.node, f)

    # Print every statement followed by a newline.
    # Only place newlines are printed
    elif isinstance(n, Stmt):
        for x in n.nodes:
            ast_print_py(x, f)
            f.write("\n")

    # Print "print" followed by the variable
    # TODO: Throw exception of more than one
    #       argument
    elif isinstance(n, Printnl):
        f.write("print ")

        if len(n.nodes) >= 1:
            ast_print_py(n.nodes[0], f)

    # Print <assigned_to>= <assigned_from>
    # Note that assigned_to is given the responsibility
    # of printing a trailing space if needed
    elif isinstance(n, Assign):
        ast_print_py(n.nodes[0], f)
        f.write("= ")
        ast_print_py(n.expr, f)

    # Print the name, followed by a trailing space
    elif isinstance(n, AssName):
        f.write(n.name + " ")

    # TODO: Figure out what Discard is
    elif isinstance(n, Discard):
        f.write("-?Discard?- ")

    # Print the name of the constant, followed by a space
    elif isinstance(n, Const):
        f.write(str(n.value) + " ")

    # Print the name raw (NO trailing space because might
    # be a function. Possibly re-evaluate this)
    elif isinstance(n, Name):
        f.write(n.name + " ")

    # Print <left>+ <right>. Left responsible for trailing space
    elif isinstance(n, Add):
        ast_print_py(n.left, f)
        f.write("+ ")
        ast_print_py(n.right, f)

    # Print -<expr>. Leading minus is tight
    elif isinstance(n, UnarySub):
        f.write("-")
        ast_print_py(n.expr, f)

    # Print <func name>([<arg[0]>[, <arg[1][...]]])
    elif isinstance(n, CallFunc):
        ast_print_py(n.node, f)
        f.write("(")

        # Print argument list. Omits comma on last arg
        if len(n.args) >= 1:
            for x in n.args[:-1]:
                ast_print_py(x, f)
                f.write(", ")
            ast_print_py(n.args[-1], f)
        f.write(") ")

    # Whoops! Wrong type of node
    else:
        raise Exception('Error in ast_num_nodes: unrecognized (P0) AST node')

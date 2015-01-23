import compiler
from compiler.ast import *
import sys

def print_prog(n):

    # Just print the whole program
    if isinstance(n, Module):
        print_prog(n.node)

    # Print every statement followed by a newline.
    # Only place newlines are printed
    elif isinstance(n, Stmt):
        for x in n.nodes:
            print_prog(x)
            print

    # Print "print" followed by the variable
    # TODO: Throw exception of more than one
    #       argument
    elif isinstance(n, Printnl):
        sys.stdout.write("print ")
        print_prog(n.nodes[0])

    # Print <assigned_to>= <assigned_from>
    # Note that assigned_to is given the responsibility
    # of printing a trailing space if needed
    elif isinstance(n, Assign):
        print_prog(n.nodes[0])
        sys.stdout.write("= ")
        print_prog(n.expr)

    # Print the name, followed by a trailing space
    elif isinstance(n, AssName):
        sys.stdout.write(n.name + " ")

    # TODO: Figure out what Discard is
    elif isinstance(n, Discard):
        sys.stdout.write("-?Discard?- ")

    # Print the name of the constant, followed by a space
    elif isinstance(n, Const):
        sys.stdout.write(str(n.value) + " ")

    # Print the name raw (NO trailing space because might
    # be a function. Possibly re-evaluate this)
    elif isinstance(n, Name):
        sys.stdout.write(n.name)

    # Print <left>+ <right>. Left responsible for trailing space
    elif isinstance(n, Add):
        print_prog(n.left)
        sys.stdout.write("+ ")
        print_prog(n.right)

    # Print -<expr>. Leading minus is tight
    elif isinstance(n, UnarySub):
        sys.stdout.write("-")
        print_prog(n.expr)

    # Print <func name>([<arg[0]>[, <arg[1][...]]])
    elif isinstance(n, CallFunc):
        print_prog(n.node)
        sys.stdout.write("(")

        # Print argument list. Ommits comma on last arg
        if len(n.args) >= 1:
            for x in n.args[:-1]:
                print_prog(x)
                sys.stdout.write(", ")
            print_prog(n.args[-1])
        sys.stdout.write(") ")

    # Whoops! Wrong type of node
    else:
        raise Exception('Error in num_nodes: unrecognized (P0) AST node')

def main():
    ast = compiler.parseFile(sys.argv[1])
    print_prog(ast)

if __name__ == "__main__":
    main()

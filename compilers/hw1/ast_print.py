import compiler
from compiler.ast import *
import sys

def ast_print_py(n, f=sys.stdout):

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

    # Discard is for an expression by itself, which won't ever be evaluated.
    # Just print the expression
    elif isinstance(n, Discard):
        ast_print_py(n.expr, f)

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
        raise Exception('Error in ast_print_py: unrecognized (P0) AST node')

def ast_print_x86(n, f=sys.stdout, vars_dict=None):
    # Module case is responsible for doing all the funtion book-keeping:
    # - How many local variables are there?
    # - Prologue
    # - Epilogue
    if isinstance(n, Module):
        # Get dictionary of variables
        vars_dict = get_vars_dict(n)
        n_vars = len(sorted(vars_dict.keys()))

        # Write prologue
        f.write(".globl _main\n")
        f.write("_main:\n")
        f.write("\tpushl %ebp\n")
        f.write("\tmovl %esp, %ebp\n")
        f.write("\tsubl $" + str(n_vars*4) + ", %esp\n")

        # Write program
        ast_print_x86(n.node, f, vars_dict)

        # Write epilogue
        f.write("\tmovl $0, %eax\n")
        f.write("\tleave\n")
        f.write("\tret\n")

    # All the statement case does is print all the sub-statements
    elif isinstance(n, Stmt):
        for node in n.nodes:
            ast_print_x86(node, f, vars_dict)

    # Print can take either a constant or a variable
    # Constant:
    # - Push the immediate value
    # - Call the function
    # - Clean up stack
    # Variable:
    # - Push the variable (from stack location)
    # - Call the function
    # - Clean up the stack
    elif isinstance(n, Printnl):
        # Check inputs
        if len(n.nodes) != 1:
            raise Exception('Error: p0: print takes exactly 1 argument')

        # Figure out what's being printed
        elif isinstance(n.nodes[0], Const):
            f.write("\tpushl $" + str(n.nodes[0].value) + "\n")
        elif isinstance(n.nodes[0], Name):
            source_position = vars_dict[n.nodes[0].name]
            f.write("\tpushl " + str(source_position) + "(%ebp)\n")
        else:
            raise Exception('Error: can only print variables or constants')

        # Call print
        f.write("\tcall _print_int_nl\n")
        f.write("\taddl $4, %esp\n")

    # Check if it's a function return value (just input) or an addition.
    # If so, call function/do addition, then grab result from eax
    # Otherwise, just put the constant or variable where
    # it needs to go
    elif isinstance(n, Assign):
        if isinstance(n.expr, CallFunc) or isinstance(n.expr, Add) or isinstance(n.expr, UnarySub):
            ast_print_x86(n.expr, f, vars_dict)
            dest_position = vars_dict[n.nodes[0].name]
            f.write("\tmovl %eax, " + str(dest_position) + "(%ebp)\n")

        elif isinstance(n.expr, Const):
            dest_position = vars_dict[n.nodes[0].name]
            f.write("\tmovl $" + str(n.expr.value) + ", " + str(dest_position) + "(%ebp)\n")

        elif isinstance(n.expr, Name):
            source_position = vars_dict[n.expr.name]
            dest_position = vars_dict[n.nodes[0].name]
            f.write("\tmovl " + str(source_position) + "(%ebp), " + str(dest_position) + "(%ebp)\n")

        else:
                raise Exception('Error: Assign only from function return values, constants, or single vaiables')

    # Should always deal with this case in Assign node. Called out individually to help with debugging
    elif isinstance(n, AssName):
        raise Exception("Should only see AssName through assignment node")

    # We only call discarded functions, because there might be other side effects
    elif isinstance(n, Discard):
        if isinstance(n.expr, CallFunc):
            ast_print_x86(n.expr, f, vars_dict)

    # Should always deal with this case elsewhere. Called out individually to help with debugging
    elif isinstance(n, Const):
        raise Exception("Should deal with constants as immediate values")

    # Should always deal with this case elsewhere. Called out individually to help with debugging
    elif isinstance(n, Name):
        raise Exception("Should deal with variables where they're used")

    # Figure out where the two vaulues live.
    # Load the first into eax, add the second into
    # eax. Leave value there for use
    elif isinstance(n, Add):
        if isinstance(n.left, Const):
            left_operand = "$" + str(n.left.value)
        elif isinstance(n.left, Name):
            left_position = vars_dict[n.left.name]
            left_operand = str(left_position) + "(%ebp)"
        else:
            raise Exception('Error: invalid addition operand')

        if isinstance(n.right, Const):
            right_operand = "$" + str(n.right.value)
        elif isinstance(n.right, Name):
            right_position = vars_dict[n.right.name]
            right_operand = str(right_position) + "(%ebp)"
        else:
            raise Exception('Error: invalid addition operand')

        f.write("\tmovl " + left_operand + ", %eax\n")
        f.write("\taddl %eax, " + left_operand + "\n")

    # Negate the operand, leave result in eax
    elif isinstance(n, UnarySub):
        source_position = vars_dict[n.expr.name]
        f.write("\tmovl " + str(source_position) + "(%ebp), %eax\n")
        f.write("\tnegl %eax\n")

    # Check the function, call the function, leave the return in eax
    elif isinstance(n, CallFunc):
        if n.node.name != "input":
            raise Exception('Error: p0: input() is the only valid function')

        f.write("\tcall _" + n.node.name + "\n")

    else:
        raise Exception('Error in ast_print_x86: unrecognized (P0) AST node')

def get_vars_dict(n):
    global n_vars
    global vars_dict

    if isinstance(n, Module):
        n_vars = 0
        vars_dict = {}

        get_vars_dict(n.node)
        return vars_dict

    elif isinstance(n, Stmt):
        for node in n.nodes:
            get_vars_dict(node)

    elif isinstance(n, Printnl):
        get_vars_dict(n.nodes[0])

    elif isinstance(n, Assign):
        get_vars_dict(n.nodes[0])
        get_vars_dict(n.expr)

    elif isinstance(n, AssName):
        if not n.name in sorted(vars_dict.keys()):
            n_vars = n_vars + 1
            vars_dict[n.name] = (-4)*n_vars

    elif isinstance(n, Discard):
        get_vars_dict(n.expr)

    elif isinstance(n, Const):
        pass

    elif isinstance(n, Name):
        if not n.name in sorted(vars_dict.keys()):
            n_vars = n_vars + 1
            vars_dict[n.name] = (-4)*n_vars

    elif isinstance(n, Add):
        get_vars_dict(n.left)
        get_vars_dict(n.right)

    elif isinstance(n, UnarySub):
        get_vars_dict(n.expr)

    elif isinstance(n, CallFunc):
        pass

    else:
        raise Exception('Error in get_vars_map: unrecognized (P0) AST node')

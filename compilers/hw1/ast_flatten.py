import compiler
from compiler.ast import *
import sys
import copy

# --- HELPER FUNCTIONS ------------------------------------------------------- #

# A module is already flat, just get its flattened list of statements
def Module_flatten(n):
    global tmp_n
    tmp_n = 0

    stmt_f = ast_flatten(n.node)
    module_f = Module(None, stmt_f)

    return module_f

# For a statement, get all its component
# flattened statements
def Stmt_flatten(n):
    nodes_f = []
    for node in n.nodes:
        nodes_f.extend(ast_flatten(node))

    stmt_f = Stmt(nodes_f)

    return stmt_f

# Flat prints:
# print
# print <Const>
# print <Name>
#
# Anything else:
# print <expr>
# ...
# flat(tmpn = <expr>)
# print tmpn
#
# and flatten tmpn = <expr>
def Printnl_flatten(n):
    global tmp_n

    # Check argument number
    if len(n.nodes) > 1:
        ast_print_py(n)
        raise Exception("Error: p0: print takes one or no arguments.")

    is_flat = (len(n.nodes) == 0 or isinstance(n.nodes[0], Const) or isinstance(n.nodes[0], Name))

    # Check for already flattened statement
    if not is_flat:
        # Create temporary variable (name and assignment target)
        tmp = Name("tmp" + str(tmp_n))
        asstmp = AssName("tmp" + str(tmp_n), 'OP_ASSIGN')
        tmp_n = tmp_n + 1

        # Create assignment and flattened assignment
        print_f = ast_flatten(Assign([asstmp], n.nodes[0]))

        # Create packaged statements
        print_f.extend([Printnl([tmp], None)])

    else:
        print_f = [copy.copy(n)]       # Must be a list so extend() works

    return print_f

# To flatten an assignment, we flatten
# the expression. The result will
# be a list of assignments ending with
# an expression.
# Finally, we create a last assignment from
# that final expression and return
# the entire list
def Assign_flatten(n):
    global tmp_n

    # Check number of arguments
    if len(n.nodes) != 1:
        ast_print_py(n)
        raise Exception("Error: p0: Assignments can be to only one variable at a time")

    assign_f = ast_flatten(n.expr)
    assign_f[-1] = Assign(n.nodes, assign_f[-1])

    return assign_f

# An assignment target is already flat.
def AssName_flatten(n):
    assname_f = [copy.copy(n)]
    return assname_f

# TODO: Figure out what a Discard node is
def Discard_flatten(n):
    discard_f = [copy.copy(n)]
    return discard_f

# A constant is already flat.
def Const_flatten(n):
    const_f = [copy.copy(n)]
    return const_f

# A variable name is already flat.
def Name_flatten(n):
    name_f = [copy.copy(n)]
    return name_f

# Flat additions:
# <Const> + <Const> (optimizable)
# <Const> + <Name>
# <Name> + <Const>
# <Name> + <Name>
#
# Otherwise: (optimize: only flatten the side(s) which need(s) it)
# <Left> + <Right>
# ...
# flat(tmpn = <Left>)
# flat(tmpm = <Right>)
# <tmpn> + <tmpm>
def Add_flatten(n):
    global tmp_n

    right_is_flat = (isinstance(n.right, Const) or isinstance(n.right, Name))
    left_is_flat  = (isinstance(n.left, Const)  or isinstance(n.left, Name))
    both_const    = (isinstance(n.left, Const) and isinstance(n.right, Const))

    if both_const:
        # Optimization to create compile-time constants
        add_f = [Const(n.left.value + n.right.value)]

    else:
        # Empty list of statements
        add_f = []

        if not left_is_flat:
            # Create temporary variable (name and assignment target)
            tmp_left = Name("tmp" + str(tmp_n))
            asstmp_left = AssName("tmp" + str(tmp_n), 'OP_ASSIGN')
            tmp_n = tmp_n + 1

            # Create an assignment target and flatten it
            add_f.extend(ast_flatten(Assign([asstmp_left], n.left)))

            # The left half of our addition is the assigned-to variable
            left_f = tmp_left

        else:
            # No need to change the left half
            left_f = copy.copy(n.left)

        if not right_is_flat:
            # Create temporary variable (name and assignment target)
            tmp_right = Name("tmp" + str(tmp_n))
            asstmp_right = AssName("tmp" + str(tmp_n), 'OP_ASSIGN')
            tmp_n = tmp_n + 1

            # Create an assignment target and flatten it
            add_f.extend(ast_flatten(Assign([asstmp_right], n.right)))

            # The right half of our addition is the assigned-to variable
            right_f = tmp_right
        else:
            # No need to change the right half
            right_f = copy.copy(n.right)

        # Create the now-flat addition, adding it
        # to whatever statements proceed
        # Not sure why the add constructor insists
        # on the extra parens, but they fix it
        add_f.append(Add([left_f, right_f]))

    # Return the flattened addition (or constant!)
    return add_f


# Check if negation operation is on flat type (name or const).
# If it's not, flatten the expression, negate the temp
def UnarySub_flatten(n):
    global tmp_n

    is_flat = (isinstance(n.expr, Name) or isinstance(n.expr, Const))

    if not is_flat:
        # Create temporary variable (name and assignment target)
        tmp = Name("tmp" + str(tmp_n))
        asstmp = AssName("tmp" + str(tmp_n), 'OP_ASSIGN')
        tmp_n = tmp_n + 1

        assign_f = ast_flatten(Assign([asstmp], n.expr))
        unarysub_f = [assign_f, UnarySub(tmp)]
    else:
        unarysub_f = [copy.copy(n)]

    return unarysub_f
        
# Since our functions take no arguments,
# a function call is always flat. Just make sure
# it's a correctly-called input()
def CallFunc_flatten(n):
    if len(n.args) != 0:
        ast_print_py(n)
        raise Exception("Error: p0: no function should have arguments")

    # I THINK this is pedantic, more of a test of the parser
    # than the code
    if not isinstance(n.node, Name):
        ast_print_py(n)
        raise Exception("Error: p0: Function should be a Name")
    
    if n.node.name != 'input':
        ast_print_py(n)
        raise Exception("Error: p0: invalid function '" + n.node.name + "' called")

    callfunc_f = [copy.copy(n)]
    return callfunc_f

def ast_flatten(n):
    global tmp_n

    if isinstance(n, Module):
        return Module_flatten(n)

    elif isinstance(n, Stmt):
        return Stmt_flatten(n)

    elif isinstance(n, Printnl):
        return Printnl_flatten(n)

    elif isinstance(n, Assign):
        return Assign_flatten(n)

    elif isinstance(n, AssName):
        return AssName_flatten(n)

    elif isinstance(n, Discard):
        return Discard_flatten(n)

    elif isinstance(n, Const):
        return Const_flatten(n)

    elif isinstance(n, Name):
        return Name_flatten(n)

    elif isinstance(n, Add):
        return Add_flatten(n)

    elif isinstance(n, UnarySub):
        return UnarySub_flatten(n)

    elif isinstance(n, CallFunc):
        return CallFunc_flatten(n)

    else:
        raise Exception('Error in ast_flatten: unrecognized (P0) AST node')

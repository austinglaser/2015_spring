import compiler
from compiler.ast import *
import sys

def ast_num_nodes(n):
    if isinstance(n, Module):
        return 1 + ast_num_nodes(n.node)
    elif isinstance(n, Stmt):
        return 1 + sum([ast_num_nodes(x) for x in n.nodes])
    elif isinstance(n, Printnl):
        return 1 + ast_num_nodes(n.nodes[0])
    elif isinstance(n, Assign):
        return 1 + ast_num_nodes(n.nodes[0]) + ast_num_nodes(n.expr)
    elif isinstance(n, AssName):
        return 1
    elif isinstance(n, Discard):
        return 1 + ast_num_nodes(n.expr)
    elif isinstance(n, Const):
        return 1
    elif isinstance(n, Name):
        return 1
    elif isinstance(n, Add):
        return 1 + ast_num_nodes(n.left) + ast_num_nodes(n.right)
    elif isinstance(n, UnarySub):
        return 1 + ast_num_nodes(n.expr)
    elif isinstance(n, CallFunc):
        return 1 + ast_num_nodes(n.node)
    else:
        raise Exception('Error in ast_num_nodes: unrecognized (P0) AST node')

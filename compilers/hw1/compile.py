#!/usr/bin/env python

# Python built-in
import compiler
from compiler.ast import *
import os
import sys
import copy

# Project
from ast_num_nodes import *
from ast_print import *
from ast_flatten import *

def main():
    if len(sys.argv) != 2:
        raise Exception("Pass only one argument, the name of the file to be compiled")

    # Get AST
    ast = compiler.parseFile(sys.argv[1])

    # Flatten AST
    ast_flat = ast_flatten(ast)

    # Get output filename
    basename = os.path.basename(sys.argv[1])
    names = basename.split('.')
    outname = '.'.join(names[:-1]) + '.s'

    out = open(outname, 'w')

    ast_print_x86(ast_flat, out)

if __name__ == "__main__":
    main()

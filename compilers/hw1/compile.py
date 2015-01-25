#!/usr/bin/env python

# Python built-in
import compiler
from compiler.ast import *
import os
import sys
import copy

# Project
from ast_num_nodes import *
from ast_print_py import *
from ast_flatten import *

def main():
    if len(sys.argv) != 2:
        raise Exception("Pass only one argument, the name of the file to be compiled")

    # Get AST
    ast = compiler.parseFile(sys.argv[1])

    # Flatten AST
    ast_flat = ast_flatten(ast)

    # Get output filename
    # Separate extension, add _flat to the remaining
    # base name, then add extension back on.
    # This will play nicely with names.with.multiple.periods.py
    # (though the result -- names.with.multiple.periods_flat.py -- is
    # not as pretty as it could be)
    basename = os.path.basename(sys.argv[1])
    names = basename.split('.')
    outname = '.'.join(names[:-1]) + '_flat.' + names[-1]

    # Write AST as python code to a file
    outfile = open(outname, 'w')
    ast_print_py(ast_flat, outfile)
    outfile.close()

if __name__ == "__main__":
    main()

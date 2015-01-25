#!/usr/bin/env python

# Python built-in
import compiler
from compiler.ast import *
import sys
import copy

# Project
from ast_num_nodes import *
from ast_print_py import *
from ast_flatten import *

def main():
    if len(sys.argv) != 2:
        raise Exception("Pass only one argument, the name of the file to be compiled")

    ast = compiler.parseFile(sys.argv[1])
    ast_flat = ast_flatten(ast)
    print
    ast_print_py(ast)
    print
    ast_print_py(ast_flat)
    print

if __name__ == "__main__":
    main()

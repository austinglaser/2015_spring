#!/usr/bin/env python

# Python built-in
import os
import sys

def main():
    if len(sys.argv) != 2:
        raise Exception("Pass only one argument, the name of the file to be compiled")

    # Open file
    infile = open(sys.argv[1])

    # Get output filename
    basename = os.path.basename(sys.argv[1])
    names = basename.split('.')
    outname = '.'.join(names[:-1]) + '.s'

    outfile = open(outname, 'w')

    for line in infile.readlines():
        outfile.write(line)

    infile.close()
    outfile.close()

if __name__ == "__main__":
    main()

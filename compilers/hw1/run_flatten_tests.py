#!/usr/bin/env python

import os
from os import listdir
import hashlib

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# https://stackoverflow.com/questions/4283639/check-files-for-equality
def filemd5(filename, block_size=2**20):
    f = open(filename)
    md5 = hashlib.md5()

    while True:
        data = f.read(block_size)
        if not data:
            break
        md5.update(data)

    f.close()
    return md5.digest()

def main():
    path = 'test/'

    # Get all file names in the path
    # https://stackoverflow.com/questions/3207219/how-to-list-all-files-of-a-directory-in-python
    path = 'test/'
    tests = [ f for f in listdir(path) if f.split('.')[-1] == 'py' ] 
    path_tests = [ path + test for test in tests]

    ints = 'test/input/ints.txt'

    for i in range(len(tests)):
        parts = tests[i].split('.')
        test = '.'.join(parts[:-1])
        flat = 'out/' + '.'.join(parts[:-1]) + '_flat.' + parts[-1]
        out = 'out/' + '.'.join(parts[:-1]) + '_out.txt'
        flat_out = 'out/' + '.'.join(parts[:-1]) + '_flat_out.txt'


        os.system('./compile.py ' + path_tests[i])
        os.system('python ' + path_tests[i] + ' < ' + ints + ' > ' + out)
        os.system('python ' + flat + ' < ' + ints + ' > ' + flat_out)

        out_md5 = filemd5(out)
        flat_out_md5 = filemd5(flat_out)

        if out_md5 == flat_out_md5:
            print bcolors.OKGREEN, '[ PASSED ] ', bcolors.ENDC, test
        else:
            print bcolors.FAIL, '[ FAILED ] ', bcolors.ENDC, + test

if __name__ == "__main__":
    main()

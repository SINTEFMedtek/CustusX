#!/usr/bin/env python

#####################################################
# Script for commiting to git with specified name, email and commit message
# Author: Janne Beate Bakeng, SINTEF Medical Technology
# Date:   2016.02.29
#
#################################################

from __future__ import print_function
import argparse
from cx.utils.cxShell import *

#################################################
#
# This idiom means the below code only runs when executed from command line
#
#################################################
def commit(name, email, message, directory):
    shell.changeDir(directory)
    shell.run('git -c user.name="'+name+'" -c user.email="'+email+'" commit -m"'+message+'"', keep_output=True, silent=False)


#################################################
#
# This idiom means the below code only runs when executed from command line
#
#################################################
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Commit using supplied name, email and message.')
    parser.add_argument('-n', '--name', required=True, help='specify your name')
    parser.add_argument('-e', '--email',required=True, help='specify your email')
    parser.add_argument('-m', '--message',required=True, help='specify your commit  message')
    parser.add_argument('-d', '--directory', required=False, help='specify directory to run the commit command in', default=os.getcwd())
    args = parser.parse_args()

    print(args)

    commit(args.name, args.email, args.message, args.directory)
    

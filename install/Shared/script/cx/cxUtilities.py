#!/usr/bin/env python

#####################################################
# 
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.06.20
#
# Description:
#
#
#####################################################
    
import os
import os.path
#import argparse
    
def writeToNewFile(filename, text):
    '''
    Write text to filename, 
    overwriting existing data.
    '''
    path = os.path.dirname(filename)
    if not os.path.exists(path):
        os.makedirs(path)
    with open(filename, 'w') as f:
        f.write(text)

def readFile(filename):
    with open(filename, 'r') as f:
        content = f.read()
    return content

def assertTrue(assertion, text):
    if not assertion:
        text = 'Test Failed: %s' % text
        print text
        raise Exception(text)

def getPathToModule():
    '''
    return path to the folder this python module resides in.
    '''
    # alternatively use  sys.argv[0] ?? 
    moduleFile = os.path.realpath(__file__)
    modulePath = os.path.dirname(moduleFile)
    modulePath = os.path.abspath(modulePath)
    return modulePath

#def argparse_add_boolean_argument(argparser, name, default, dest, help):
#    '''
#    Variant of argparse.ArgumentParser.add_argument().
#    Add a zero-argument option to enable/disable the attribute 'dest',
#    depending on its default value.
#    '''
#    print 'Adding option', name, default
#    if default==True:
#        argparser.add_argument('--skip_%s'%name, action='store_false', dest=dest, help=help)
#    else:
#        argparser.add_argument('--%s'%name, action='store_true', dest=dest, help=help)

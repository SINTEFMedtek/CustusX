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

def assertTrue(assertion, text):
    if not assertion:
        text = 'Test Failed: %s' % text
        print text
        raise Exception(text)


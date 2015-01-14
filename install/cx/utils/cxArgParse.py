#!/usr/bin/env python

#####################################################
# 
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.10.11
#
#
#####################################################
    
import argparse

class ArgumentParser(argparse.ArgumentParser):    
    '''
    '''
    def add_boolean_inverter(self, name, default, dest, help):
        '''
        Variant of argparse.ArgumentParser.add_argument().
        Add a zero-argument option to enable/disable the attribute 'dest',
        depending on its default value.
        '''
        name = name.lstrip('-')
        #print 'Adding option', name, default
        if default==True:
            self.add_argument('--skip_%s'%name, action='store_false', dest=dest, help=help)
        else:
            self.add_argument('--%s'%name, action='store_true', dest=dest, help=help)

#!/usr/bin/env python3

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
        Add both --name and --skip_name options for the attribute 'dest'.
        '''
        name = name.lstrip('-')
        self.add_argument('--%s'%name, action='store_true', dest=dest, help=help)
        self.add_argument('--skip_%s'%name, action='store_false', dest=dest, help=help)
        self.set_defaults(**{dest: default})

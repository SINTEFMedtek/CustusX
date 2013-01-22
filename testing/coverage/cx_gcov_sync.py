#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.01.18
#
# Description:
#
#
#             
#################################################             

import subprocess
import optparse
import re
import sys
import os.path
import urllib
import getpass
import platform
import glob
import os
import shutil
import pprint



def main():
    '''
    Find all gcda files in the input dir, recursively.
    Find all gcno files in same dirs.
    For each gcda file, find the corresponding gcno file
    and copy it into the folder of the gcda file.
    '''
    parser = optparse.OptionParser(description="cx gcov stuff",
                              version='%prog version 0.2',
                              usage= '%prog [options] [components]')
    options, arguments = parser.parse_args()
    if len(arguments)==0:
        return
    print "argument: ", arguments[0]

    # find all the gcda files
    gcdaFiles = []
    for r,d,f in os.walk(arguments[0]):
            for files in f:
                if files.endswith(".gcda"):
                    #print os.path.join(r,files)
                    gcdaFiles.append((r,files))

    # find all the gcno files
    gcnoFiles = []
    for r,d,f in os.walk(arguments[0]):
        for files in f:
            if files.endswith(".gcno"):
                #print os.path.join(r,files)
                gcnoFiles.append((r,files))
    print "Found %i gcno files" % len(gcnoFiles)

    # for each gcda, find a gcno and copy it into the folder of gcda
    for gcda in gcdaFiles:
        currentGcno = '.'.join(gcda[1].split('.')[:-1]) + '.gcno'
        result = [file for file in gcnoFiles if file[1]==currentGcno]

        if len(result)==0:
            print "Found no gcno file for %s/%s" % (gcda[0], gcda[1])
            continue

        # check for duplicates
        if len(result)>1:
            print "Warning: Multiple .gcno files detected for %s/%s: " % (gcda[0], gcda[1])
            for one in result:
                print '\t%s/%s' % (one[0],one[1])

        # check for existence in the gcda folder
        inplace = False
        for one in result:
            if one[0] == gcda[0]:
                print "gcno file already in place for %s/%s, ignoring" % (gcda[0], gcda[1])
                inplace = True
        if inplace==True:
            continue

        # find the gcno file
        foundGcno = None
        for one in result:
            if one[0] != gcda[0]:
                foundGcno = one
        if foundGcno==None:
            print "Error: Found no suitable gcno for %s/%s, ignoring" % (gcda[0], gcda[1])
            continue
    
        src = foundGcno[0]+'/'+foundGcno[1]
        dst = gcda[0]+'/'+foundGcno[1]
        
        print "Copy [%s] from [%s] to [%s]" % (foundGcno[1], foundGcno[0], gcda[0])
        shutil.copy(src,dst)



#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()


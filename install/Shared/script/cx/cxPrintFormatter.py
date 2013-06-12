#!/usr/bin/env python

#####################################################
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.06.07
#
# Description:
#
#################################################             

import time

class PrintFormatterClass:                    
    def __init__(self):
        self._startTime = time.time()

    def printHeader(self, caption, level=3):
        elapsed = "%.1fs" % (time.time() -self._startTime)
        if level < 3:
            self.printLine()
        else:
            print ''

        # create a line, 80 wide, containing caption and elapsed        
        part1 ='='*20
        part2 = (" %s "%caption).ljust(50, '=')
        part3 = (" %s "%elapsed).ljust(10, '=')
        print '%s%s%s' % (part1, part2, part3)

        #print '%s%s%s' % ('='*20, (" %s "%caption).ljust(50, '='), (" %s "%elapsed).ljust(10, '='))
        for i in range(3-level):
            self.printLine()
            
    def printLine(self):
        print '='*80

    def finish(self):
        self.printHeader('Finished', level=3)
        
PrintFormatter = PrintFormatterClass()

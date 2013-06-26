#!/usr/bin/env python

#####################################################
# Catch helper
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.06.19
#
# Description:
#
#
#################################################

import sys
import re
import pprint

class CatchConsoleNameListParser:
    '''
    '''
    def __init__(self):
        self.lines = []
        self.numberOfTests = -1
        self.tests = []
        pass
    
    def readTestNamesFromStringList(self, lines):
        self.lines = lines
#        self._printLines("Input lines")
        self.numberOfTests = self._readNumberOfTestsFromFooter()
        self._removeHeaderAndFooter()
        self._findTestsFromLines()
#        self._printTests("Tests")
        self.validate()
        
    def isValid(self):
        return int(self.numberOfTests) == len(self.tests)
  
    def validate(self):      
        if not self.isValid():
            print "Number of tests from footer: %s" % self.numberOfTests
            print "Number of tests from parse: %s" % len(self.tests)
            print "Parser failed."
            exit(1)
        
    def _printLines(self, header):
        self._printHeader(header)
        for line in self.lines:
            print "%s" % line.strip('\n')
            #print "<%s>" % line.strip('\n')

    def printTests(self, header):
        self._printHeader(header)
        for index,test in enumerate(self.tests):
            print "#%d: %s" % (index+1, test)
        print ""
        print "%d tests." % len(self.tests)
        
    def _printHeader(self, header):
        print ''
        print "=== %s ===" % header 
        print ''

    def _removeHeaderAndFooter(self):
        startLine = self._findLineIndexAfterHeader()
        endLine = self._findLineIndexOfFooter()
        self.lines = self.lines[startLine:endLine]

    def _findLineIndexAfterHeader(self):
        startSymbol = self._getHeaderRegexp()
        for index,line in enumerate(self.lines):
            if startSymbol.match(line):
                return index+1
        return len(self.lines)

    def _findLineIndexOfFooter(self):
        endSymbol = self._getFooterRegexp()
        for index,line in reversed(list(enumerate(self.lines))):
            match = endSymbol.match(line)
            if match:
                return index
        return 0
    
    def _readNumberOfTestsFromFooter(self):
        endSymbol = self._getFooterRegexp()
        for line in reversed(self.lines):
            match = endSymbol.match(line)
            if match:
                return int(match.group(1))
        return -1
    
    def _getFooterRegexp(self):
        return re.compile(r'([\d]+) test cases')

    def _getHeaderRegexp(self):
        return re.compile(r'All available test cases:')
    
    def _getTestLineRegexp(self):
        '''
        Match this format <enclosed in brackets>:
        <  Description of test  .....  [several][tags]>
        <    additional desc             [more][tags]>
        <    additional desc             [more][tags]>
        <      ...                       [evenmore][tags]>
        and extract "Description of test additional desc additional desc"
        as the "text" group.
        The "subline_identifier" group is non-None if the line is a subline, 
        None if first line.
        '''
        return re.compile(r'''
            [ ]{2}                            # two whitespace header
            (?P<subline_identifier>[ ]{2})?   # two whitespace subline header, will be None if first line. 
            (?P<text>.*?)                     # all text (nongreedy)
            [.]*                              # dot fill
            [ ]*                              # whitespace separator name - tags
            (?P<tags>(\[.*?\])*)              # any number of [tag], tag text nongreedy.
            $                                 # end of line
            ''', re.VERBOSE)

    def _findTestsFromLines(self):
        regexp = self._getTestLineRegexp()
        for line in self.lines:
            self._appendOneLineToTestList(line, regexp)

    def _appendOneLineToTestList(self, line, regexp):
        match = regexp.match(line)
        sublineIdentifier = match.group('subline_identifier')
        text = match.group('text').strip()
        tags = match.group('tags')
        #print "MATCH:<%s><%s>" % (sublineIdentifier, text)
        #print "TAGS: %s" % tags
        testinfo = {'text':text, 'tags':tags}
        if sublineIdentifier:
            self._appendTextToLastText(testinfo)
        else:
            self._appendNewTest(testinfo)

    def _appendNewTest(self, testinfo):
        self.tests.append(testinfo)

    def _appendTextToLastText(self, testinfo):
        #self.tests[-1] = "%s %s" % (self.tests[-1], text)
        if len(testinfo['text'])>0:
            self.tests[-1]['text'] = "%s %s" % (self.tests[-1]['text'], testinfo['text'])
        self.tests[-1]['tags'] = "%s%s" % (self.tests[-1]['tags'], testinfo['tags'])

    def getTestsForTag(self, tag):
        return [val['text'] for val in self.tests if val['tags'].find(tag)>=0]

# =============================================================================
# =============================================================================

def main():
    lines = sys.stdin.readlines() 
    parser = CatchConsoleNameListParser()
    name_list = parser.readTestNamesFromStringList(lines)
    parser.printTests("Tests")

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()
 
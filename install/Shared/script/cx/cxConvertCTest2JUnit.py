#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.06.23
#
# Description:
#
# Based on :
#   http://stackoverflow.com/questions/6329215/how-to-get-ctest-results-in-hudson-jenkins
#
# Prerequisites:
#   run "sudo pip install lxml" to install required py module.
#   mac
#   sudo easy_install pip
#   Ubuntu: sudo apt-get install -y python-pip libxml2-dev libxslt-dev 
#
#################################################             

import cxUtilities
import StringIO
import sys

def transformXML(sourceXml, xlsTransform):
    '''
    Transform the source xml string using the 
    xls transform string, return result.
    '''  
    lxml = cxUtilities.try_lxml_import()
  
    xmldoc = lxml.etree.parse(StringIO.StringIO(sourceXml))
    xslt_root = lxml.etree.XML(xlsTransform)
    transform = lxml.etree.XSLT(xslt_root)
    
    result_tree = transform(xmldoc)
    return result_tree

def transformXMLFile2XMLFile(sourceXml, xlsTransform, outfile):
    '''
    Convert the input xml file using 
    the input xls transform file.
    Write output to outfile
    '''
    source = cxUtilities.readFile(sourceXml)
    transform = cxUtilities.readFile(xlsTransform)
    result_tree = transformXML(source, transform)
    cxUtilities.writeToNewFile(filename=outfile, text=str(result_tree))

def convertCTestFile2JUnit(ctestFile, junitFile):
    xlstFile = '%s/ctest2junit.xsl' % cxUtilities.getPathToModule()
    transformXMLFile2XMLFile(ctestFile, xlstFile, junitFile)

if __name__ == '__main__':
    TAGfile = open(sys.argv[1]+"/Testing/TAG", 'r')
    dirname = TAGfile.readline().strip()
        
    sourceXml = sys.argv[1]+"/Testing/"+dirname+"/Test.xml"
    xlsTransform = sys.argv[2]

    result_tree = transformXML(cxUtilities.readFile(sourceXml), cxUtilities.readFile(xlsTransform))
    
    print(result_tree)

#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#             
#################################################             

import cx.build.cxComponentAssembly
import cx.script.cxInstallScript

class Controller(cx.script.cxInstallScript.Controller):
    '''
    Installer script for CustusX
    '''
    def __init__(self, assembly=None):
        ''                
        assembly = cx.build.cxComponentAssembly.LibraryAssembly()
        super(Controller, self).__init__(assembly)

if __name__ == '__main__':
    controller = Controller()
    controller.run()


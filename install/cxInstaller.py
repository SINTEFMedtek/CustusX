#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#             
#################################################             

import sys
import cx.build.cxInstallData
import cx.build.cxComponentAssembly
import cx.script.cxInstallScript

class Controller(cx.script.cxInstallScript.Controller):
    '''
    Installer script for CustusX
    '''
    def __init__(self, assembly=None):
        ''
        # Pre-parse CLI flags so the assembly sees the correct mBuildIGSTK value
        # before its __init__ runs. Without this, LibraryAssembly() always uses
        # the default (False), so the IGSTK conditional never takes effect.
        controlData = cx.build.cxInstallData.Common()
        controlData.applyCommandLine(sys.argv[1:])
        assembly = cx.build.cxComponentAssembly.LibraryAssembly(controlData)
        super(Controller, self).__init__(assembly)

if __name__ == '__main__':
    controller = Controller()
    controller.run()


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
        # Set mBuildIGSTK before LibraryAssembly.__init__ so the correct
        # components (oldVTK/ITK/IGSTK) are added to the library list.
        # Then reset to False so add_boolean_inverter registers --igstk
        # (not --skip_igstk) when the parsers are built in super().__init__.
        # applyCommandLine() re-applies --igstk from sys.argv afterwards.
        controlData = cx.build.cxInstallData.Common()
        controlData.mBuildIGSTK = '--igstk' in sys.argv
        assembly = cx.build.cxComponentAssembly.LibraryAssembly(controlData)
        controlData.mBuildIGSTK = False
        super(Controller, self).__init__(assembly)

if __name__ == '__main__':
    controller = Controller()
    controller.run()


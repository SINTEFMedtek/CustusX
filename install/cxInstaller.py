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
        # Check --igstk early so LibraryAssembly.__init__ sees the correct
        # mBuildIGSTK and includes oldVTK/ITK/IGSTK in the component list.
        # Full argument parsing happens in super().__init__(); we only set
        # this one flag here to avoid changing the add_boolean_inverter
        # default before the parsers are built.
        controlData = cx.build.cxInstallData.Common()
        if '--igstk' in sys.argv:
            controlData.mBuildIGSTK = True
        assembly = cx.build.cxComponentAssembly.LibraryAssembly(controlData)
        super(Controller, self).__init__(assembly)

if __name__ == '__main__':
    controller = Controller()
    controller.run()


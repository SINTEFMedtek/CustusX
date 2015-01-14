#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.10.13
#
# Description:
#
#
#####################################################


import cx.script.cxJenkinsMasterBuildScript

class Controller(cx.script.cxJenkinsMasterBuildScript.Controller):
    '''
    '''
    pass
        
if __name__ == '__main__':
    controller = Controller()
    controller.run()

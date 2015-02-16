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


import cx.script.cxJenkinsBuildScript_JobDefinitions

class Controller(cx.script.cxJenkinsBuildScript_JobDefinitions.Controller):
    '''
    '''
    pass
        
if __name__ == '__main__':
    controller = Controller()
    controller.run()

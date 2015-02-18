#!/usr/bin/env python

#####################################################
# Unix jenkins script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2015.02.16
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

#!/usr/bin/env python

#####################################################
# Jenkins control script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.12.09
#
# Description:
#
#   Create jenkins jobs for several platforms based
#   on templates.    
#   This is a replacement for the Jenkins Matrix job.
#
#
#####################################################

import cxCreateRelease
#import cx.cxUtilities
import time
import argparse
from cx.cxPrintFormatter import PrintFormatter

import pkg_resources
print 'jenkinsapi version:', pkg_resources.get_distribution("jenkinsapi").version

class JobGenerator:
    def __init__(self, jenkins):
        self.jenkins = jenkins
        self.api = self.jenkins.jenkins
#        self.lxml = cx.cxUtilities.try_lxml_import()
        self.verbose = False
        
    def enableJob(self, base_jobname, target):
        new_jobname = base_jobname.replace('_base_', target)
#        time.sleep(1) # give last call time to complete
        self.api.get_job(new_jobname).enable()
        pass

    def duplicateJob(self, base_jobname, target):
        '''
        retrieve the config.xml of job(jobname),
        then replace old target(s) with new one(s).
        set: Project name       in function call
             Description        <project/description> 
             Custum Workspace   <project/customWorkspace>
             Label Expression   <project/assignedNode>
             Downstream projects
            
        '''
        if (not self.api.has_job(base_jobname)):
            print 'failed to duplicate job %s, job does not exist' % base_jobname
            return

        print 'duplicating job %s for target %s' % (base_jobname, target)  
        base_config = self._get_job_configuration(base_jobname)
        self.printConfig(base_config)
        new_config = self._update_config(base_config, target)
        self.printConfig(new_config)
        new_jobname = base_jobname.replace('_base_', target)
        self._set_job_configuration(new_jobname, new_config)
        
    def printConfig(self, config):
        if self.verbose:
            print '*' * 50
            print config
            print '*' * 50

    def _get_job_configuration(self, jobname):
        job = self.api.get_job(jobname)
#        print help(job)
        config = job.get_config()
        return config
        
    def _set_job_configuration(self, jobname, config):
        if (self.api.has_job(jobname)):
            print 'Reconfiguring %s' % jobname 
            self.jenkins.pyjenkins.reconfig_job(jobname, config)
        else:            
            print 'Creating %s' % jobname
            self.api.create_job(jobname, config)
            # self.api.create_job(new_jobname, config)
            # self.api.reconfig_job(new_jobname, config)

    def _update_config(self, base_config, target):
        return base_config.replace('_base_', target)
#        etree = self.lxml.etree
#        root = etree.fromstring(base_config)
#        self.appendNodeText(root, 'description', "<p>Using target %s</p>" % target);        
#        self.setNodeText(root, 'assignedNode', target);
# #        self.setNodeText(root, 'project/customWorkspace', 'ws/%s'%target); // attempt with default
        return etree.tostring(root, xml_declaration=True)

    def setNodeText(self, root, tag, text):
        node = root.find(tag)
        node.text = text

    def appendNodeText(self, root, tag, text):
        node = root.find(tag)
        node.text = node.text + text
    #############################################

#configureJobs()
# getHelp()

class Controller:#(cx.cxBuildScript.BuildScript):
    '''
    '''
    def getDescription(self):                  
        return '''\
   Create jenkins jobs for several platforms based
   on templates.    
   This is a replacement for the Jenkins Matrix job.
   
   Templates: %s
   Targets: %s
''' % (', '.join(self.basejobs), ', '.join(self.targets))

    def __init__(self):
        self.basejobs = ['unit__base_', 'integration__base_', 'unstable__base_']
        self.targets = ['ubuntu.12.04.x64', 'macosx.10.8', 'win32', 'win64', 'ubuntu.12.04.x64.amd']    
        self.argumentParser = self.getArgParser()
        self.options = self.argumentParser.parse_args()
        pass

    def getArgParser(self):
        p = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
                                    description=self.getDescription())
        p.add_argument('-u', '--username', default="user", help='jenkins user')
        p.add_argument('-p', '--password', default="not set", help='jenkins password')
        return p
                
    def run(self):
        PrintFormatter.printHeader('Create jenkins jobs',level=1)
        PrintFormatter.printInfo('Templates: %s' % self.basejobs)
        PrintFormatter.printInfo('Targets: %s' % self.targets)
        self._configureJobs()            
        PrintFormatter.printHeader('Finished',level=3)

    def _configureJobs(self):
        '''
        Generate jenkins jobs for all targets based on the templates
        '''
        jenkins = self._getJenkinsConnection()
        
        generator = JobGenerator(jenkins)
        generator.verbose = False
        
        for basejob in self.basejobs:
            for target in self.targets:
                generator.duplicateJob(basejob, target)
    
        print 'enabling jobs...'
        time.sleep(2)  # give last call time to complete
        for basejob in self.basejobs:
            for target in self.targets:
                generator.enableJob(basejob, target)
    
    def getHelp(self):
        '''
        utility function for extracting help from the jenkinsapi
        '''
        jenkins = self._getJenkinsConnection()
        jobname = 'unit__base_'
        job = jenkins.jenkins.get_job(jobname)
        help(job)
        
    def _getJenkinsConnection(self):
        jenkins = cxCreateRelease.Jenkins()
        jenkins.username = self.options.username
        jenkins.password = self.options.password
        jenkins.initializeJenkins()
        return jenkins
    
if __name__ == '__main__':
    controller = Controller()
    controller.run()

        

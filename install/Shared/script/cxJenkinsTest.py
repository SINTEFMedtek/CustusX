#!/usr/bin/env python
#import logging
#from jenkinsapi import api
#import jenkinsapi
import cxCreateRelease
import cx.cxUtilities


import pkg_resources
print 'version:', pkg_resources.get_distribution("jenkinsapi").version

class JobGenerator:
    def __init__(self, jenkins):
        self.jenkins = jenkins
        self.api = self.jenkins.jenkins
        self.lxml = cx.cxUtilities.try_lxml_import()
        
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
        print 'duplicating job %s for target %s' % (base_jobname, target)  
        base_config = self._get_job_configuration(base_jobname)
        #print 'Original config:'
        #print config
#        print help(base_job)
        #print help(self.api)
        new_config = self._update_config(base_config, target)
        print new_config
        new_jobname = base_jobname.replace('_base', "_"+target)
        print new_jobname
        self._set_job_configuration(new_jobname, new_config)

    def _get_job_configuration(self, jobname):
        job = self.api.get_job(jobname)
        config = job.get_config()
        return config
        
    def _set_job_configuration(self, jobname, config):
        if (self.api.has_job(jobname)):
            print 'reconfiguring %s' % jobname 
            self.jenkins.pyjenkins.reconfig_job(jobname, config)
        else:            
            print 'creating %s' % jobname
            self.api.create_job(jobname, config)
            #self.api.create_job(new_jobname, config)
            #self.api.reconfig_job(new_jobname, config)

    def _update_config(self, base_config, target):
        #print base_config
        etree = self.lxml.etree
        root = etree.fromstring(base_config)
        #print "root ", root.text
        #print etree.tostring(root)
        
        #print '*'*50
        #for element in root.iter():
        #    print '    tag:%s, text:%s' % (element.tag, element.text)
        
        print '*'*50
        desc_node = root.find('description')
        desc_node.text = desc_node.text + "<p>Using target %s</p>" % target
        print '*'*50
        #print "root: ", root.text
        project_node = etree.SubElement(root, 'project')
        #print "proj/: ", project_node.text
        desc_node = etree.SubElement(root, 'description')
        #print "proj/desc: ", desc_node, desc_node.tag, desc_node.text
        #root = self.lxml.etree.XML(base_config)        
        #tree = etree.ElementTree(root)
        
        #print "VER: ", tree.docinfo.xml_version
        return etree.tostring(root)


jenkins = cxCreateRelease.Jenkins()
jenkins.password = 'christiana'
jenkins.initializeJenkins()

#jobname = 'CustusX'
#jobname = 'CustusX_unit'
jobname = 'unit_base'
target = 'ubuntu.12.04.x64'
job = jenkins.jenkins.get_job(jobname)

#print job.get_config()
#help(job)
generator = JobGenerator(jenkins)
generator.duplicateJob(jobname, target)


        
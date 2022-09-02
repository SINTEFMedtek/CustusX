from __future__ import print_function
#####################################################
# 
# Used by both CustusX and applications built on top of it (wrapper applications).
# Intended to be copied into the repos of wrapper applications.
#
#####################################################

from builtins import input
from builtins import object
import os.path
import os
import sys
import subprocess
import pprint
import argparse

def returnCode():
    return 0

def runShell(cmd, path):
    '''
    simple shell implementation.
    Return value is last stdout line, None if failure.
    Note: May return an empty string that indicated success, but evaluates to False.
    '''
    if not os.path.exists(path):
        os.makedirs(path)
    print('[shell cmd] %s [%s]' % (cmd, path))
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=path)
    out, err = p.communicate("") # wait for process to complete
    if type(err) is bytes:
        err = err.decode()
    if err:
        print(err.strip())
    if out:
        print(out.strip())
    if p.returncode == 0:
        return out.strip()
    if "error: The following untracked working tree files would be overwritten" in err:
        return returnCode()
    return None

def getBranchForRepo(path, fallback=None):
    'Find the git branch in the repo in the input path, None if detached'
    branch = runShell('git rev-parse --abbrev-ref HEAD', path)
    if branch=="HEAD":
        return fallback
    return branch


class RepoHandler(object):
    '''
    Handle git repository URL, tag, branch when checking out and updating the repo
    tasks:
     - checkout/set correct URL, configurable from code and command line
     - checkout correct tag, configurable from command line
     - checkout correct branch configurable from code and command line
     -  fallback policy when branch does not exist
    '''
    pass

    def __init__(self, silent=True):
        self.silent = silent
        self.default_branch = None
        self.fallback_branch = 'develop'
        self.args = self._parseArgs()

    def setRepoInfo(self, url_base=None, url_name=None, root_path=None, repo_path=None):
        'Set repo properties. Those not set cannot be used and attempt to do so will cause failure.'
        self.url_base = url_base
        self.url_name = url_name
        self.root_path = root_path
        self.repo_path = repo_path

    def cloneRepoWithPrompt(self):
        '''
        Checkout the repository, clone if needed.
        requires:
         - url
         - checkout folder
         - root path
        '''
        pathfound = os.path.exists(self.repo_path)
        if pathfound:
            return
        
        print('*** %s will be cloned in [%s]' % (self.getName(), self.root_path))
        doprompt = not (self.silent or args.silent_mode)
        self._promptToContinue(doprompt)
        
        runShell('git clone %s %s' % (self.getUrl(), self.repo_path), self.root_path)
            
    def getUrl(self):
        url_base = self.url_base
        if self.args.gitrepo_main_site_base:
            url_base = self.args.gitrepo_main_site_base
        url = '%s/%s' % (url_base, self.url_name) 
        return url
    
    def getName(self):
        return self.repo_path.split('/')[-1]
        
    def syncToGitRef(self):
        '''
        Checkout the correct tag or branch    
        - if git_tag is set, use that, else fail
        - if main_branch is set, use that, else:
        -   try the default and fallback branches
        '''
        runShell('git fetch', self.repo_path)

        tag = self.args.git_tag
        if tag:
            print('Checking out %s to tag=%s' % (self.getName(), tag))
            if runShell('git checkout %s' % tag, self.repo_path) is None:
                exit("tag checkout failed")
            return
        
        branches = [self.args.main_branch, 
                    self.default_branch,
                    self.fallback_branch]
        branches = self.cleanBranchList(branches)

        print('Checkout+pull {} to to the first existing branch in list [{}]'.format(self.getName(), ','.join(branches)))
        
        for branch in branches:
            result = runShell('git checkout %s' % branch, self.repo_path)
            self.checkSuccess(result)
            if result is not None:
                result = runShell('git pull origin %s' % branch, self.repo_path)
                self.checkSuccess(result)
                break

    def checkSuccess(self, gitResult):
        if gitResult is returnCode():
            print('----------------------------------------------------------------------------')
            print('|                                     ^                                    |')
            print('| Delete the folder containing these files and the CustusX build folder.   |')
            print('----------------------------------------------------------------------------')
            print('===== Your local file structure of CustusX is not in synch with the file structure on the server ===')
            print('This is e.g. because a plugin have been moved from one repository to another on the server.')
            print('Take a backup first if you have been working with the files.')
            print('To come in sync:')
            print('- delete the folder containing the above mentioned files and the CustusX build folder.')
            print('- run the script again.')
            sys.exit(1)

    def cleanBranchList(self, branches):
        retval = []
        for branch in branches:
            if branch and branch not in retval:
                if type(branch) is bytes:
                    branch = branch.decode()
                #print("{} is TYPE: {}".format(branch, type(branch)))
                retval.append(branch)
        return retval
        
    def setBranchDefault(self, branch):
        '''
        set a default branch to use if no main_branch is set
        '''
        self.default_branch = branch
    
    def setBranchFallback(self, branch):
        '''
        Set the branch to use if no other branches work
        '''
        self.fallback_branch = branch

    def _parseArgs(self):
        parser = argparse.ArgumentParser(add_help=False, conflict_handler='resolve')
        parser.add_argument('-g', '--git_tag', default=None, metavar='TAG', dest='git_tag')
        parser.add_argument('--main_branch', default=None, dest='main_branch')
        parser.add_argument('--gitrepo_main_site_base', default=None)
        args = parser.parse_known_args()[0]
        return args
    
    
    def _promptToContinue(self, do_it):
        if do_it:
            input("\nPress enter to continue or ctrl-C to quit:")
    

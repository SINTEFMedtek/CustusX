#!/usr/bin/env python

#####################################################
# Object representing a git repository
# Author: Janne Beate Bakeng, SINTEF Medical Technology
# Date:   2014.08.20
#
# Description:
#
#             
#################################################

from __future__ import print_function
from builtins import object
import argparse
import os
import fnmatch
from cx.utils.cxShell import *
import re
import os.path

#################################################
#
# Finds all directories containing a .git folder
# NB: Time consuming. 
#
#################################################
def findGitRepositories(path):
    print("Looking for git repositories in \""+root_path+"\"")
    repositories = []
    for root, dirnames, filenames in os.walk(path):
        for dirname in fnmatch.filter(dirnames, '.git'):
            repositories.append(GitRepository(root))
    return repositories


#################################################
#
# Represents a git repository. 
#
#################################################
class GitRepository(object):
    def __init__(self, path):
        self.path = path
        self.clean = False
        self.modified_files = False
        self.deleted_files = False
        self.untracked_files = False
        self.renamed_files = False
        self.branch_name = ""
        self.branch_ahead = ""
        
    def checkout_branch(self, branch_name):
        return_value = self.__run_git_command('checkout %s' % branch_name)
        
    def pull(self):
        return_value = self.__run_git_command('pull')
        
    def push(self):
        return_value = self._run_git_command('push')
        
    def update_status(self):
        #status
        text = self.__run_git_command('status').stdout
        
        if 'working directory clean' in text:
            self.clean = True
        if 'modified:' in text:
            self.modified_files = True
        if 'deleted:' in text:
            self.deleted_files = True
        if 'Untracked files:' in text:
            self.untracked_files = True
        if 'renamed:' in text:
            self.renamed_files = True
            
        #TODO the next two git commands are not needed, the same info can be found in 'git status'
        
        #branch_ahead
        text = self.__run_git_command('status --porcelain -b').stdout
        hit = re.match(r'##(.*)\n', text)
        if hit:
            if hit.group(1):
                text = hit.group(1)
                
        branch_ahead = ''
        hit = re.match(r'.*\[(.*)\].*', text)
        if (hit and hit.group(1)):
            branch_ahead = hit.group(1)
        
        text = text.split('...')[0]
        self.branch_ahead = branch_ahead
        
        #branch_name
        self.branch_name = self.__run_git_command('rev-parse --abbrev-ref HEAD').stdout.strip()
        
    def __run_git_command(self, command, silent=True):
        shell.setSilent(silent)
        shell.changeDir(self.path)
        return shell.run('git '+command, keep_output=True, silent=silent)
        

#################################################
#
# Defines colors used to format info on the commandline 
# http://misc.flogisoft.com/bash/tip_colors_and_formatting
#
#################################################
class TextColor(object):
    if(platform.system() == 'Windows'):
        INFO = ''
        CLEAN = ''
        DIRTY = ''
        MODIFIED = ''
        DELETED = ''
        UNTRACKED = ''
        RENAMED = ''
        ENDC = ''
    else:
        INFO = '\033[95m' #light magenta
        CLEAN = '\033[0m' #white
        DIRTY = '\033[1m' #bold
        MODIFIED = '\033[94m' #light blue
        DELETED = '\033[91m' #light red
        UNTRACKED = '\033[96m' #light cyan
        RENAMED = '\033[92m' #light green
        ENDC = '\033[0m' #white

#################################################
#
# Provides pretty formatted information. 
#
#################################################      
class Reporter(object):
    def __init__(self, root_path):
        self.root_path = root_path

    def print_status(self, repo):
        if(not repo):
            return
            
        repo.update_status()
        b = repo.branch_name
        p = self.__get_repo_path(repo)
        d = self.__get_repo_details(repo)
        a = '*'
        if('[]' in d):
            a = ''
        print('{0:<1}  {1:<70}  {2:<30}  {3}'.format(a, p, b, d))
    
    def __get_repo_path(self, repo):
        path = os.path.relpath(repo.path, self.root_path)        
        if repo and not repo.branch_ahead:
            return TextColor.CLEAN + path
        else:
            return TextColor.DIRTY + path
    
    def __get_repo_details(self, repo):
         return self.__format_details(repo).ljust(50)
        
    def __format_details(self, repo):
        text = TextColor.ENDC 
        text += '['
        if repo.modified_files:
            text += TextColor.MODIFIED + ' modified ' + TextColor.ENDC 
        if repo.deleted_files:
            text += TextColor.DELETED + ' deleted ' + TextColor.ENDC
        if repo.untracked_files:
            text += TextColor.UNTRACKED + ' untracked ' + TextColor.ENDC
        if repo.renamed_files:
            text += TextColor.RENAMED + ' renamed ' + TextColor.ENDC
        if repo.branch_ahead:
            text += TextColor.MODIFIED + ' %s '%repo.branch_ahead + TextColor.ENDC
        text += ']' 
        text += TextColor.ENDC
        
        return text
    
    def __print_no_repositories_found(self):
        print(TextColor.ENDC + "No repositories found..." + TextColor.ENDC)
    
    def __print_info(self, text):
        print(TextColor.INFO + text + TextColor.ENDC)
        
    def __print_newline(self):
        print(" ")

#################################################
#
# This idiom means the below code only runs when executed from command line
#
#################################################
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Display condensed git status for all projects.')
    parser.add_argument('root_path', help='the top directory to recursively look for git repositories in (abs or rel)') 
    parser.add_argument('-c', '--command', help='specify what you want to do', action='append', choices=['status', 'pull', 'push', 'checkout'], default=[])
    parser.add_argument('-b', '--branch', help='specify which branch you want', default='master')
    args = parser.parse_args()
    
    print(args)
    
    root_path = os.path.abspath(args.root_path)
    git_repositories = findGitRepositories(root_path)
    reporter = Reporter(args.root_path)
    
    if('checkout' in args.command):
        pass
    if(not args.command or 'status' in args.command):
        for repo in git_repositories:
            reporter.print_status(repo)
    if('pull' in args.command):
        pass
    if('push' in args.command):
        pass

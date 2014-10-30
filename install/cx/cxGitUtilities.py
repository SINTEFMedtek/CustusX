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

import argparse
import os
import fnmatch
from cxShell import *
import re
import os.path

#################################################
#
# Finds all directories containging a .git folder
# NB: Timeconsuming. 
#
#################################################
def findGitRepositories(path):
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
        self.name = os.path.basename(self.path)
        self.clean = False
        self.modified_files = False
        self.deleted_files = False
        self.untracked_files = False
        self.renamed_files = False
        
    def get_name(self):
        return self.name
        
    def status(self):
        return_value = self.__run_git_command('status', True)
        self.__evaluate(return_value)
        
    def branch_info(self):
        text = self.__run_git_command('status --porcelain -b', True).stdout
        hit = re.match(r'##(.*)\n', text)
        if hit:
            if hit.group(1):
                text = hit.group(1)
                
        branch_ahead = ''
        hit = re.match(r'.*\[(.*)\].*', text)
        if (hit and hit.group(1)):
            branch_ahead = hit.group(1)
        
        text = text.split('...')[0]
        self.branch_name = text
        self.branch_ahead = branch_ahead
        
    def __run_git_command(self, command, silent):
        shell.setSilent(silent)
        shell.changeDir(self.path)
        return shell.run('git '+command, keep_output=True, silent=silent)
    
    def __evaluate(self, return_value):
        if 'working directory clean' in return_value.stdout:
            self.clean = True
        if 'modified:' in return_value.stdout:
            self.modified_files = True
        if 'deleted:' in return_value.stdout:
            self.deleted_files = True
        if 'Untracked files:' in return_value.stdout:
            self.untracked_files = True
        if 'renamed:' in return_value.stdout:
            self.renamed_files = True
        

#################################################
#
# Defines colors used to format info on the commandline 
# http://misc.flogisoft.com/bash/tip_colors_and_formatting
#
#################################################
class TextColor:
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
# Provides pretty formated information. 
#
#################################################      
class Reporter(object):
    def __init__(self, root_path):
        self.root_path = root_path
        self.__print_info("Looking for git repositories in \""+root_path+"\"")
        self.git_repositories = findGitRepositories(root_path)

    def print_status(self):
        if(not self.git_repositories):
            self.__print_no_repositories_found()
            return
            
        for repo in self.git_repositories:
            repo.status()
            repo.branch_info()
            b = repo.branch_name 
            p = self.__get_repo_path(repo)
            d = self.__get_repo_details(repo)
            a = '*'
            if('[]' in d):
                a = ''
            print '{0:<1}  {1:<70}  {2:<30}  {3}'.format(a, p, b, d)
    
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
        print TextColor.ENDC + "No repositories found..." + TextColor.ENDC
    
    def __print_info(self, text):
        print TextColor.INFO + text + TextColor.ENDC
        
    def __print_newline(self):
        print " "

#################################################
#
# This idiom means the below code only runs when executed from command line
#
#################################################
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Display condensed git status for all projects.')
    parser.add_argument('root_path', help='the top directory to recursively look for git repositories in (abs or rel)') 
    args = parser.parse_args()
    
    root_path = os.path.abspath(args.root_path)
    reporter = Reporter(root_path)
    reporter.print_status()
    

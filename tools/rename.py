#!/usr/bin/env python

#####################################################
# Renames cplusplus file pairs, changes include
# guard and include header statement
#
# Author: Janne Beate Bakeng, SINTEF Medical Technology
# Date:   2013.06.07
#
# Description:
#
# Scipt takes absolute or relative path to a header file
# and base name it should be renamed to.
#
# Warning: only tested on Ubuntu 12.04 with Python 2.7.3 
# 
# Example: 
#     $> ./rename.py ./source/something.h somethingelse
#
#####################################################

import sys, glob, os
import fileinput
import re
import argparse
import shutil
import difflib

#copied from http://stackoverflow.com/questions/3041986/python-command-line-yes-no-input
def query_yes_no(question, default="yes"):
    """Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is one of "yes" or "no".
    """
    valid = {"yes":True,   "y":True,  "ye":True,
             "no":False,     "n":False}
    if default == None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "\
                             "(or 'y' or 'n').\n")

def copy_file(src_abs_path, dst_abs_path):
    shutil.copyfile(src_abs_path, dst_abs_path)
    
def delete_file(file_abs_path):
    os.remove(file_abs_path)
    return

def rename_file_keep_extension(abs_file_path, new_base_name):
    file = File(abs_file_path)
    new_abs_file_path = os.path.normpath(file.get_folder_path()+"/"+new_base_name+file.get_extension())
    os.rename(file.get_absolute_path(), new_abs_file_path)
    return new_abs_file_path

def rename_include_guard(header_file, new_base_name):
    include_guard_regex = '\S*._H_' # = any number of non-white chars followed by _H_
    new_include_guard = (new_base_name+"_H_").upper()
    find_and_replace_text_in_file(header_file, include_guard_regex, new_include_guard)
    return
    
def rename_include_header_file(source_file, old_header_file_abs_path, header_extension, new_base_name):
    include_header_regex = '#include \"'+os.path.basename(old_header_file_abs_path)+'\"'
    new_include_header = '#include "'+new_base_name+header_extension+'"'
    find_and_replace_text_in_file(source_file, include_header_regex, new_include_header)
    return
    
def find_and_replace_text_in_file(abs_file_path, regex_pattern, replace_with_text):
    for line in fileinput.input(abs_file_path, inplace = True):
        sys.stdout.write(re.sub(regex_pattern, replace_with_text, line))
    return

'''
Gives infomation about a file.

Example:
abs/or/rel/path/to/test.h
    get_absolute_path -> abs/path/to/test.h
    get_folder_path -> abs/path/to
    get_name -> test.h
    get_base_name -> test
    get_extension -> .h
'''
class File:
    def __init__(self, path_to_file):
        self.exists = os.path.isfile(path_to_file)
        if(not self.exists):
            raise RuntimeError("File does not exist: "+path_to_file)
        self.path_to_file = path_to_file
        return
       
    def get_absolute_path(self):
        return os.path.abspath(self.path_to_file)
     
    def get_folder_path(self):
        return os.path.dirname(self.get_absolute_path())
        
    def get_name(self):
        return os.path.basename(self.get_absolute_path())
    
    def get_base_name(self):
        file_name = self.get_name()
        base, ext = os.path.splitext(file_name)
        return base
        
    def get_extension(self):
        root, ext = os.path.splitext(self.get_absolute_path())
        return ext
    
'''
Will present information about a cplusplus header and source pair.

Example:
test.h and test.cpp
    isValid -> true if both files exists
    get_header_extension -> .h
    get_source_extension -> .cpp
    get_header_file -> abs/path/to/test.h
    get_source_file -> abs/path/to/test.cpp
'''
class CppFilePair:
    def __init__(self, path_to_header_file):
        self.header_extensions = [".h"]
        self.source_extensions = [".cpp"]
        abs_path = os.path.abspath(path_to_header_file)
        self._find_cpp_file_pair(abs_path)
        
    def is_valid(self):
        return os.path.isfile(self.get_header_file()) and os.path.isfile(self.get_source_file())
    
    def get_header_extension(self):
        file = File(self.get_header_file())
        return file.get_extension()
    
    def get_source_extension(self):
        file = File(self.get_source_file())
        return file.get_extension()
    
    def get_header_file(self):
        return self.header_file_abs_path
    
    def get_source_file(self):
        return self.source_file_abs_path
    
    def rename_pair(self, new_base_name):
        self.header_file_abs_path = rename_file_keep_extension(self.get_header_file(), new_base_name)
        self.source_file_abs_path = rename_file_keep_extension(self.get_source_file(), new_base_name)
        return
    
    def update_include_guard(self, new_base_name):
        rename_include_guard(self.get_header_file(), new_base_name)
        return
    
    def update_include_header(self, new_base_name):
        rename_include_header_file(self.get_source_file(), self.old_header_file_abs_path, self.get_header_extension(), new_base_name)
        return
    
    def backup(self):
        self.old_header_file_abs_path = self.header_file_abs_path
        self.header_file_abs_path_backup = self._backup_file(self.header_file_abs_path)
        self.old_source_file_abs_path = self.source_file_abs_path
        self.source_file_abs_path_backup = self._backup_file(self.source_file_abs_path)
        print '[FILES BACKED UP]'
        return
        
    def restore_from_backup(self):
        if(not self._is_backed_up()):
            return
        delete_file(self.header_file_abs_path)
        delete_file(self.source_file_abs_path)
        copy_file(self.header_file_abs_path_backup, self.old_header_file_abs_path)
        copy_file(self.source_file_abs_path_backup, self.old_source_file_abs_path)
        self.header_file_abs_path = self.old_header_file_abs_path
        self.source_file_abs_path = self.old_source_file_abs_path
        print '[FILES RESTORED]'
        return
        
    def delete_backup(self):
        delete_file(self.header_file_abs_path_backup)
        delete_file(self.source_file_abs_path_backup)
        return
        
    def print_diff(self):
        self._diff(self.header_file_abs_path_backup, self.header_file_abs_path)
        self._diff(self.source_file_abs_path_backup, self.source_file_abs_path)
        return
        
    def _diff(self, fromfile_abs_path, tofile_abs_path):
        fromlines = open(fromfile_abs_path, "r").readlines()
        tolines = open(tofile_abs_path, "r").readlines()
        diff = difflib.unified_diff(fromlines, tolines, fromfile_abs_path, tofile_abs_path)
        sys.stdout.writelines(diff)
        return
    
    def _find_cpp_file_pair(self, path_to_file):
        file = File(path_to_file)
        self.header_file_abs_path = self._find_header_file(file)
        self.source_file_abs_path = self._find_source_file(file)
        return
        
    def _find_header_file(self, file):
        list = self._find_files_in_folder(file.get_folder_path(), file.get_base_name(), self.header_extensions)
        if(len(list) < 1):
            raise RuntimeError("Could not find a header file.")
        header = os.path.normpath(file.get_folder_path()+"/"+list[0])
        return header
    
    def _find_source_file(self, file):
        list = self._find_files_in_folder(file.get_folder_path(), file.get_base_name(), self.source_extensions)
        if(len(list) < 1):
            raise RuntimeError("Could not find a source file.")
        source = os.path.normpath(file.get_folder_path()+"/"+list[0])
        return source
        
    def _find_files_in_folder(self, folder_path, file_base_name, extension_list):
        os.chdir(folder_path)
        relative_file_list = []
        for extension in extension_list:
            relative_file_list += glob.glob(file_base_name + extension)
        return relative_file_list
    
    def _is_backed_up(self): 
        return os.path.isfile(self.header_file_abs_path_backup) and os.path.isfile(self.source_file_abs_path_backup) 
    
    def _backup_file(self, file_abs_path):
        file = File(file_abs_path)
        backup_file_abs_path = ".BACKUP"+file.get_extension()
        copy_file(os.path.realpath(file_abs_path), os.path.realpath(backup_file_abs_path))
        return backup_file_abs_path
    
'''
Renames a cplusplus file pair.
This means changing:
    - the include guard in the header file
    - the include header in the source file
    - the name of both the header and source file
    
Example:
file_pair contains test.h & test.cpp
renamer(file_pair, "new_name")
    renamer.rename():
        --> #ifndef ... TEST_H_ -> #ifndef ... NEW_NAME_H_
        --> #include "test.h" -> #include "new_name.h"
        --> test.h -> new_name.h
        --> test.cpp -> new_name.cpp
'''
class CppFileRenamer():
    def __init__(self, cpp_file_pair, new_base_name):
        if(not cpp_file_pair.is_valid()):
            raise RuntimeError("Header/source file pair is not valid, cannot rename.")
        self.cpp_file_pair = cpp_file_pair
        self.new_base_name = new_base_name
        return

    def rename(self):
        self.cpp_file_pair.backup()
        self.cpp_file_pair.rename_pair(self.new_base_name)
        self.cpp_file_pair.update_include_guard(self.new_base_name)
        self.cpp_file_pair.update_include_header(self.new_base_name)
        self._ask_if_keep_changes()
        return 
                   
    def _ask_if_keep_changes(self):
        self._print_changes()
        answere = query_yes_no("\nKeep these changes?")
        if(not answere):
            self.cpp_file_pair.restore_from_backup()
        self.cpp_file_pair.delete_backup()
        return 
        
    def _print_changes(self):
        self.cpp_file_pair.print_diff()
        return
    
'''
Parses the scripts incoming arguments and does what it's told.
'''
def main():
    argv_parser = argparse.ArgumentParser(description='Rename a cplusplus header and source file pair.')
    argv_parser.add_argument("header_file", help="Absolute or relative path to existing cplusplus header file. ex: ./thing.h", type=str)
    argv_parser.add_argument("new_name", help="New base name for the header file. ex: something", type=str)
    argv_parser.add_argument("-d", "--debug", action = "store_true", help="Used for debugging the script")
    
    args = argv_parser.parse_args()
    
    if(args.debug):
        return
    
    file_pair = CppFilePair(args.header_file)
    renamer = CppFileRenamer(file_pair, args.new_name)
    renamer.rename()
    
    return
    

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()
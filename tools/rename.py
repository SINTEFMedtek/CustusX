#!/usr/bin/env python

#####################################################
# Renames cplusplus file pairs, changes include
# guard and include header statement
#
# Author: Janne Beate Bakeng, SINTEF Medical Technology
# Date:   2013.06.07
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.11.02
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

class Logger:
    def __init__(self):
        pass
    def setVerbosityLevel(self, level):
        '''
        0 = quiet
        1 = errors 
        2 = warnings
        3 = status
        '''
        self.verbosity_level = level
    def setRootPath(self, root_path):
        self.root_path = root_path
    def logFileLine(self, file, line, message):
        if self.verbosity_level < 3:
            return
        rel = os.path.relpath(file, self.root_path)
        print '  [%s:%s]    %s' % (rel, line, message)
    def log(self, message):
        if self.verbosity_level < 3:
            return
        print message
    def error(self, message):
        if self.verbosity_level < 1:
            return
        print message
    def warning(self, message):
        if self.verbosity_level < 2:
            return
        print message
logger = Logger()
    

def copy_file(src_abs_path, dst_abs_path):
    shutil.copyfile(src_abs_path, dst_abs_path)
    
def delete_file(file_abs_path):
    os.remove(file_abs_path)
    return

def get_renamed_filename(abs_file_path, new_base_name):
    file = File(abs_file_path)
    new_abs_file_path = os.path.normpath(file.get_folder_path()+"/"+new_base_name+file.get_extension())
    return new_abs_file_path

def rename_file_keep_extension(abs_file_path, new_base_name):
    new_abs_file_path = get_renamed_filename(abs_file_path, new_base_name)
    os.rename(abs_file_path, new_abs_file_path)
    return new_abs_file_path

def find_include_guard_id(header_file):
    search_regex = '^#ifndef (\S*._H_?)$'
    matcher = re.compile(search_regex)
    with open(header_file, 'r') as f:
        for line in f.readlines():
            match = matcher.match(line)
            if match:
                return match.group(1)

def rename_include_guard(header_file, new_base_name):
    include_guard_regex = find_include_guard_id(header_file)
    if not include_guard_regex:
        logger.logFileLine(header_file, 0, "No include guard found")
        return 
    new_include_guard = (new_base_name+"_H_").upper()
    find_and_replace_text_in_file(header_file, include_guard_regex, new_include_guard)
    
def rename_include_header_file(source_file, old_header_file_abs_path, header_extension, new_base_name):
    include_header_regex = '#include (\"|<)'+os.path.basename(old_header_file_abs_path)+'(\"|>)'
    new_include_header = '#include "'+new_base_name+header_extension+'"'
    find_and_replace_text_in_file(source_file, include_header_regex, new_include_header)
    return
    
def find_and_replace_text_in_file(abs_file_path, regex_pattern, replace_with_text):
    if not os.path.exists(abs_file_path):
        return
    with open(abs_file_path, 'r') as f:
        old = f.readlines()
    with open(abs_file_path, 'w+') as f:
        for (index, line) in enumerate(old):
            (modified_line, count) =re.subn(regex_pattern, replace_with_text, line)
            if count>0:
                logger.logFileLine(abs_file_path, index, 'Modified %s -> %s' % (regex_pattern, replace_with_text))
            f.write(modified_line)    

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

    def rename(self, new_base_name, file_repository):
        if  not self.at_least_one_file_existing_and_renamable(new_base_name):
            return        
        if self.get_header_file():
            self.rename_header(new_base_name, file_repository)
        if self.get_source_file():
            self.rename_source(new_base_name, file_repository)
        
    def rename_header(self, new_base_name, file_repository):
        logger.log('Renaming %s to base %s' % (self.get_header_file(), new_base_name))
        self.header_file_abs_path = rename_file_keep_extension(self.get_header_file(), new_base_name)
        self.update_include_guard(new_base_name)
        self.update_include_header_in_files(new_base_name, file_repository.get_cpp_files())
        self.update_cmakelists(new_base_name, file_repository.get_cmakelists())

    def rename_source(self, new_base_name, file_repository):
        logger.log('Renaming %s to base %s' % (self.get_source_file(), new_base_name))
        self.source_file_abs_path = rename_file_keep_extension(self.get_source_file(), new_base_name)
        self.update_cmakelists(new_base_name, file_repository.get_cmakelists())

    def at_least_one_file_existing_and_renamable(self, new_base_name):
        has_header = self._is_file(self.get_header_file()) 
        has_source = self._is_file(self.get_source_file())
        if not has_header and not has_source:
            logger.error('At least one of source/header must be present for rename to %s' % new_base_name)
            return False
        if has_header:
            if not self.validate_target_not_exists(self.get_header_file(), new_base_name):
                return False
        if has_source:
            if not self.validate_target_not_exists(self.get_source_file(), new_base_name):
                return False
        return True

    def _is_file(self, file):
        if not file:
            return False
        return os.path.isfile(file)
    
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

    def validate_target_not_exists(self, old_file, new_base_name):
        new_file = get_renamed_filename(old_file, new_base_name)
        if os.path.exists(new_file):
            logger.error('Error: %s already exists. Cannot rename to existing file' % new_file)
            return False
        return True
    
    def update_include_guard(self, new_base_name):
        rename_include_guard(self.get_header_file(), new_base_name)
        return
    
    def update_include_header_in_files(self, new_base_name, target_files):
        for target_file in target_files:
            #print 'target_file, ', target_file
            rename_include_header_file(target_file, self.old_header_file_abs_path, self.get_header_extension(), new_base_name)            
    
    def update_cmakelists(self, new_base_name, target_files):
        if self.old_header_file_abs_path:
            old_base_name = os.path.basename(self.old_header_file_abs_path)
        else:
            old_base_name = os.path.basename(self.old_source_file_abs_path)            
        old_base_name = os.path.splitext(old_base_name)[0]
        for target_file in target_files:
            find_and_replace_text_in_file(target_file, regex_pattern=old_base_name, replace_with_text=new_base_name)
        
    def _find_cpp_file_pair(self, path_to_file):
        file = File(path_to_file)
        self.header_file_abs_path = self._find_header_file(file)
        self.source_file_abs_path = self._find_source_file(file)
        self.old_header_file_abs_path = self.header_file_abs_path
        self.old_source_file_abs_path = self.source_file_abs_path
        return
        
    def _find_header_file(self, file):
        list = self._find_files_in_folder(file.get_folder_path(), file.get_base_name(), self.header_extensions)
        if(len(list) < 1):
            logger.warning("Could not find a header file %s/%s." % (file.get_folder_path(), file.get_base_name()))
            return None
        header = os.path.normpath(file.get_folder_path()+"/"+list[0])
        return header
    
    def _find_source_file(self, file):
        list = self._find_files_in_folder(file.get_folder_path(), file.get_base_name(), self.source_extensions)
        if(len(list) < 1):
            logger.warning("Could not find a source file  %s/%s." % (file.get_folder_path(), file.get_base_name()))
            return None
        source = os.path.normpath(file.get_folder_path()+"/"+list[0])
        return source
        
    def _find_files_in_folder(self, folder_path, file_base_name, extension_list):
        os.chdir(folder_path)
        relative_file_list = []
        for extension in extension_list:
            relative_file_list += glob.glob(file_base_name + extension)
        return relative_file_list
        
'''
Renames a cplusplus file pair.
This means changing:
    - the include guard in the header file
    - the include header in all files 
    - the name of both the header and source file
    - CMakeLists.txt add_target()

One or both og source and header need be present.    

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
    def __init__(self, cpp_file_pair, new_base_name, file_repository):
        self.cpp_file_pair = cpp_file_pair
        self.new_base_name = new_base_name
        self.file_repository = file_repository
        return

    def rename(self):
        self.cpp_file_pair.rename(self.new_base_name, self.file_repository)

class FileRepository():
    def __init__(self, root_dir):
        self.root_dir = os.path.abspath(root_dir)
        #self.cpp_files = self._find_all_matching(self._get_cppfiles_regexp())
        #self.cmakelists = self._find_all_matching(self._get_cmakelists_regexp())
    
    def get_cpp_files(self):
        return self._find_all_matching(self._get_cppfiles_regexp())
        #return self.cpp_files

    def get_cmakelists(self):
        return self._find_all_matching(self._get_cmakelists_regexp())
        #return self.cmakelists
    
    def get_ssc_header_files(self):
        return self._find_all_matching(self._get_sscfiles_regexp())
    
    def _find_all_matching(self, regexp):
        retval = []
        for root, dirs, files in os.walk(self.root_dir):
            for file in files:
                if regexp.match(file):
                    fullpath = os.path.abspath('%s/%s' % (root, file))
                    retval.append(fullpath)
        return retval

    def _get_cppfiles_regexp(self):
        extensions = ['cpp', 'h']
        ext_rx = [('.*\.%s$')%e for e in extensions]
        expr = '(%s)' % '|'.join(ext_rx)
        return re.compile(expr)

    def _get_cmakelists_regexp(self):
        return re.compile('CMakeLists.txt')
    
    def _get_sscfiles_regexp(self):
        return re.compile('ssc([a-zA-Z_0-9]*.[a-zA-Z_0-9]*)$')

def rename_ssc_to_cx(file_repository):
    ssc_files = file_repository.get_ssc_header_files();
    for ssc_file in ssc_files:
        if not os.path.exists(ssc_file):
            continue
        file_pair = CppFilePair(ssc_file)
        pattern = r'(.*)ssc([a-zA-Z_0-9]*\.[a-zA-Z_0-9]*)$'
        cx_file = re.sub(pattern, r'\1cx\2', ssc_file)
        cx_file = os.path.basename(cx_file)
        cx_file = os.path.splitext(cx_file)[0]
        logger.log("%s -> %s" % (ssc_file, cx_file))
        renamer = CppFileRenamer(file_pair, cx_file, file_repository)
        renamer.rename()
    pass
'''
Parses the scripts incoming arguments and does what it's told.
'''
def main():
    argv_parser = argparse.ArgumentParser(description='Rename a cplusplus header and source file pair.')
    argv_parser.add_argument("header_file", help="Absolute or relative path to existing cplusplus header file. ex: ./thing.h", type=str)
    argv_parser.add_argument("new_name", help="New base name for the header file. ex: something", type=str)
    argv_parser.add_argument("--root_dir", help="root directory, work on all files inside", type=str)
    argv_parser.add_argument("-d", "--debug", action = "store_true", help="Used for debugging the script")
    argv_parser.add_argument("-v", "--verbosity", default=2, type=int, help="Verbosity level: 0=quiet, 1=errors, 2=warnings, 3=status")
    #argv_parser.add_argument('--verbose', '-v', action='count', help='Verbosity: -v: errors+warning, -vv: ')
    args = argv_parser.parse_args()
    
    if(args.debug):
        return
    
    file_repository = FileRepository(args.root_dir)
    logger.setRootPath(os.path.abspath(args.root_dir))
    logger.setVerbosityLevel(args.verbosity)

    rename_ssc_to_cx(file_repository)
    return
    
    file_pair = CppFilePair(args.header_file)
    renamer = CppFileRenamer(file_pair, args.new_name, file_repository)
    renamer.rename()
    
    return
    

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()
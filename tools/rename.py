#!/usr/bin/env python

import sys, glob, os
import fileinput 

class FileInfo:
    def __init__(self, path_to_file):
        self.exists = os.path.isfile(path_to_file)
        if(not self.exists):
            raise RuntimeError("File does not exist: "+path_to_file)
        self.path_to_file = path_to_file
        self.absolute_file_path = self.get_absolute_file_path()
        self.folder_path = self.get_folder_path()
        self.file_name = self.get_file_name()
        self.file_base_name = self.get_file_base_name()
        self.file_extension = self.get_file_extension()
       
    def get_absolute_file_path(self):
        return os.path.abspath(self.path_to_file)
     
    def get_folder_path(self):
        return os.path.dirname(self.absolute_file_path)
        
    def get_file_name(self):
        return os.path.basename(self.absolute_file_path)
    
    def get_file_base_name(self):
        file_name = self.get_file_name()
        base, ext = os.path.splitext(file_name)
        return base
        
    def get_file_extension(self):
        root, ext = os.path.splitext(self.absolute_file_path)
        return ext

class CppFilePair:
    def __init__(self, path_to_header_file):
        self.header_extensions = [".h"]
        self.source_extensions = [".cpp"]
        self._find_cpp_file_pair(path_to_header_file)
        
    def is_valid(self):
        return os.path.isfile(self.get_header_file()) and os.path.isfile(self.get_source_file())
    
    def get_header_file(self):
        return self.header_file
    
    def get_source_file(self):
        return self.source_file
        
    def _find_cpp_file_pair(self, path_to_file):
        file_info = FileInfo(path_to_file)
        self.header_file = self._find_header_file(file_info)
        self.source_file = self._find_source_file(file_info)
        
    def _find_header_file(self, info):
        list = self._find_files_in_folder(info.folder_path, info.file_base_name, self.header_extensions)
        if(len(list) < 1):
            raise RuntimeError("Could not find a header file.")
        header = os.path.normpath(info.folder_path+"/"+list[0])
        return header
    
    def _find_source_file(self, info):
        list = self._find_files_in_folder(info.folder_path, info.file_base_name, self.source_extensions)
        if(len(list) < 1):
            raise RuntimeError("Could not find a source file.")
        source = os.path.normpath(info.folder_path+"/"+list[0])
        return source
        
    def _find_files_in_folder(self, folder_path, file_base_name, extension_list):
        os.chdir(folder_path)
        relative_file_list = []
        for extension in extension_list:
            relative_file_list += glob.glob(file_base_name + extension)
        
        return relative_file_list
    
class CppFileRenamer():
    def __init__(self, cpp_file_pair, new_base_name):
        if(not cpp_file_pair.is_valid()):
            raise RuntimeError("Header/source file pair is not valid, cannot rename.")
        self.cpp_file_pair = cpp_file_pair
        self.new_base_name = new_base_name

    def rename(self):
        self._rename_include_guard(self.cpp_file_pair.get_header_file())
        self._rename_include_header_file(self.cpp_file_pair.get_source_file())
        self._rename_file_keep_extension(self.cpp_file_pair.get_header_file())
        self._rename_file_keep_extension(self.cpp_file_pair.get_source_file())
        
    def _rename_file_keep_extension(self, abs_file_path):
        file_info = FileInfo(abs_file_path)
        new_abs_file_path = os.path.normpath(file_info.folder_path+"/"+self.new_base_name+file_info.file_extension)
        os.rename(file_info.absolute_file_path, new_abs_file_path)

    def _rename_include_guard(self, header_file):
        print "TODO: rename include guard not working as intended yet"
        return
        info = FileInfo(header_file)
        include_guard_regex = "\S*._H_" # = any number of non-white chars followed by _H_
        new_include_guard = self.new_base_name+"_H_"
        new_include_guard = new_include_guard.upper()
        self._find_and_replace_text(header_file, include_guard_regex, new_include_guard)
        
    def _rename_include_header_file(self, source_file):
        print "TODO: rename include header file"
        
    def _find_and_replace_text(self, abs_file_path, find_pattern, replace_with_text):
        print abs_file_path
        print find_pattern
        print replace_with_text
        for line in fileinput.input(abs_file_path, inplace = True):
          sys.stdout.write(line.replace(find_pattern, replace_with_text))
          

def main():
    #Scipt takes absolute or relative path to a header file
    #and base name it should be renamed to
    #example: 
    #    $> ./rename.py ./source/something.h somethingelse
    
    path_to_header_file  = sys.argv[1]
    new_base_name = sys.argv[2]
       
    file_pair = CppFilePair(path_to_header_file)
    print "Found header file: "+file_pair.get_header_file()
    print "Found source file: "+file_pair.get_source_file()
    
    renamer = CppFileRenamer(file_pair, new_base_name)
    renamer.rename()
    

#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()
#!/usr/bin/env python

#####################################################
# Private build script
# Author: Janne Beate Bakeng, SINTEF Medical Technology
# Date:   2013.05.21
#
# Description:
#
# Script for running a private build
#
#####################################################
import subprocess
from collections import deque

def getTimeFormat():
    format = "\n\t---SUMMARY---\n\t[COMMAND] %C \n\t[TIME] %E real, %U user, %S sys \n\t[EXIT] %x"
    return format

def log(message):
    print(message)
    
    
def getTimedCommandList(command):
    command_list = ["time", "-f "+getTimeFormat()]
    while (command):
        command_list.append(command.pop(0))
    return command_list
    
def run(command):
    log("\t==========================")
    command_list = getTimedCommandList(command)
    code = subprocess.call(command_list)
    log("\t[EXIT STATUS] "+str(code))
    log("\t==========================\n")
    return code

def printIntro():
    print('\n<<<<<<<<<< Private build script >>>>>>>>>>\n\t         *author*\n\t    Janne Beate Bakeng\n')

def testStreaming():
    return deque(["make -j8".split(" "),
                   "./source/testing/Catch -g [streaming] -s".split(" "),
                   "./source/plugins/Acquisition/testing/cxTestAcquisition_CppUnit_CTest -r".split(" "),
                   "./source/resource/videoServer/testing/cxTestGEInterface_CppUnit_CTest -r".split(" ")
                   ])
    
def testStreamingWidgets():
    return deque(["make -j8".split(" "),
                   "./source/testing/Catch -g [widget] -s".split(" ")#,
#                    "./source/apps/CustusX/CustusX".split(" ")
                   ])

def main():
    printIntro()
    command_queue = testStreamingWidgets()
    
    while(command_queue):
        code = run(command_queue.popleft())
        if(code != 0):
            log("[FAILURE] exiting")
            return

    print("[SUCCESS]")
    return
    
#This idiom means the below code only runs when executed from command line
if __name__ == '__main__':
    main()
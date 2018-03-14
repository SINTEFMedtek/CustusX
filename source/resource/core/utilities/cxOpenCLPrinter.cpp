/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOpenCLPrinter.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

namespace cx
{

void OpenCLPrinter::printPlatformAndDeviceInfo()
{
	VECTOR_CLASS<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	VECTOR_CLASS<cl::Device> devices;
    for(unsigned int i = 0; i < platforms.size(); i++)
    {
    	printPlatformInfo(platforms[i]);

		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

		for(unsigned int j = 0; j < devices.size(); j++)
		{
            printDeviceInfo(devices[j]);
		}
    }
    print("Number of platforms", platforms.size());
    print("Number of devices", devices.size());
}

void OpenCLPrinter::printPlatformInfo(cl::Platform platform)
{
	print("--- PlatformInfo ---", "");
	print("Name", platform.getInfo<CL_PLATFORM_NAME>());
	print("Vendor", platform.getInfo<CL_PLATFORM_VENDOR>());
	print("Version", platform.getInfo<CL_PLATFORM_VERSION>());
	print("Profile", platform.getInfo<CL_PLATFORM_PROFILE>());
	print("Extensions", "");
	printStringList(platform.getInfo<CL_PLATFORM_EXTENSIONS>());
}

void OpenCLPrinter::printDeviceInfo(cl::Device device, bool verbose)
{
	print("--- DeviceInfo ---", "");
	print("Name", device.getInfo<CL_DEVICE_NAME>());
	print("Vendor", device.getInfo<CL_DEVICE_VENDOR>());
	print("Vendor id", device.getInfo<CL_DEVICE_VENDOR_ID>());
	print("Device supports", device.getInfo<CL_DEVICE_VERSION>());
	print("Graphics card driver", device.getInfo<CL_DRIVER_VERSION>());
	print("Available", (device.getInfo<CL_DEVICE_AVAILABLE>() ? "Yes" : "No"));
	print("Extensions", "");
	printStringList(device.getInfo<CL_DEVICE_EXTENSIONS>());
	print("\n","");

	if(!verbose)
		return;

	//Verbose output
	print("Compute Units", device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>());
	print("Clock Frequency (MHz)", device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>());
	print("Global Memory (MB)", (double)device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()/1048576);
	print("Max Allocateable Memory (MB)", (double)device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>()/1048576);
	print("Local Memory (KB)", device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>());
	print("\n","");

	print("Device Profile", device.getInfo<CL_DEVICE_PROFILE>());
	print("Error correction support", device.getInfo<CL_DEVICE_ERROR_CORRECTION_SUPPORT>() ? "Yes" : "No");
	print("Profiling Timer Resolution", device.getInfo<CL_DEVICE_PROFILING_TIMER_RESOLUTION>());
	print("Endian Little", device.getInfo<CL_DEVICE_ENDIAN_LITTLE>() ? "Yes" : "No");
	print("Command queue properties", device.getInfo<CL_DEVICE_QUEUE_PROPERTIES>());
	print("Execution capabilities", device.getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>());
	print("Host unified memory", device.getInfo<CL_DEVICE_HOST_UNIFIED_MEMORY>() ? "Yes" : "No");
	print("\n","");

	print("Max work group size", device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>());
	int maxWorkItemDimensions = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>();
	print("Max work item dimensions", maxWorkItemDimensions);
	print("\n","");

	print("Image support", device.getInfo<CL_DEVICE_IMAGE_SUPPORT>() ? "Yes" : "No");
	print("Image2D max width", device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>());
	print("Image2D max height",device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>());
	print("Image3D max width", device.getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH>());
	print("Image3D max height", device.getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT>());
	print("Image3D max depth", device.getInfo<CL_DEVICE_IMAGE3D_MAX_DEPTH>());
}

void OpenCLPrinter::printContextInfo(cl::Context context)
{
	print("--- ContextInfo ---", "");
	VECTOR_CLASS<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	print("Number of devices", devices.size());
	for(int i=0; i<devices.size(); ++i)
		printDeviceInfo(devices[i]);
}

void OpenCLPrinter::printProgramInfo(cl::Program program)
{
	print("--- ProgramInfo ---", "");
	//printProgramSource(program);
}

void OpenCLPrinter::printProgramSource(cl::Program program)
{
	print("--- ProgramSource ---", "");
	cl::STRING_CLASS source = program.getInfo<CL_PROGRAM_SOURCE>();
	print("", "\n"+source);
}

void OpenCLPrinter::printKernelInfo(cl::Kernel kernel)
{
	print("--- KernelInfo ---", "");
	cl::STRING_CLASS functionName = kernel.getInfo<CL_KERNEL_FUNCTION_NAME>();
	cl::Context context = kernel.getInfo<CL_KERNEL_CONTEXT>();
	cl::Program program = kernel.getInfo<CL_KERNEL_PROGRAM>();
	print("Function name", functionName);
	printContextInfo(context);
	printProgramInfo(program);

}

void OpenCLPrinter::printMemoryInfo(cl::Memory memory)
{
	print("--- MemoryInfo ---", "");
	cl::Context context = memory.getInfo<CL_MEM_CONTEXT>();
	printContextInfo(context);
}

void OpenCLPrinter::printStringList(std::string list, std::string separator)
{
	std::vector<std::string> strings;
	boost::split(strings, list, boost::is_any_of(std::string(separator)));
	std::vector<std::string>::iterator it;
	for(it = strings.begin(); it != strings.end(); ++it)
		print("", (*it));
}

void OpenCLPrinter::print(std::string name, std::string value, int indents)
{
	std::string stringIndents = getIndentation(indents);
    std::cout << stringIndents << boost::format("%-30s %-20s\n") % name % value;
}

void OpenCLPrinter::print(std::string name, int value, int indents)
{
	std::string stringValue = boost::lexical_cast<std::string>(value);
	print(name, stringValue, indents);
}

std::string const OpenCLPrinter::getIndentation(unsigned int numberOfIndents)
{
	std::string indentator = "\t";
	std::string retval("");
	for(unsigned int i=0; i < numberOfIndents; i++)
		retval += indentator;
	return retval;
}

} //namespace cx

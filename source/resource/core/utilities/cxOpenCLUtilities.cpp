#ifdef SSC_USE_OpenCL

#include "cxOpenCLUtilities.h"
#include <stdio.h>
#include <vector>
#include <boost/algorithm/string.hpp>


namespace cx
{
#define MAX_CHAR_LENGTH 1024
#define MAX_NUMBER_OF_PLATFORMS 32
#define MAX_NUMBER_OF_DEVICES 32
#define MAX_WORK_ITEM_DIMENSIONS 3
#define MAX_NUMBER_OF_PROPERITES 30
#define MAX_SOURCE_LENGTH 1048576
#define DEFAULT_INDENTATION "\t"

void OpenCLUtilities::printPlatformAndDeviceInfo()
{
	//Based on:
	//https://devtalk.nvidia.com/default/topic/498968/printing-all-opencl-devices-not-detecting-all-opencl-devices-under-windows/

	cl_platform_id platformIds[MAX_NUMBER_OF_PLATFORMS];	//an array to hold the IDs of all the platforms, hopefuly there won't be more than 32
	cl_uint numOfPlatforms;									//this number will hold the number of platforms on this machine
	cl_device_id devices[MAX_NUMBER_OF_DEVICES];			//this variable holds the number of devices for each platform, hopefully it won't be more than 32 per platform
	cl_uint numOfDevices;									//this number will hold the number of devices on this machine

	printf("\n============================   OpenCL   ============================\n\n");

	unsigned int indent = 1;
	clGetPlatformIDs (MAX_NUMBER_OF_DEVICES, platformIds, &numOfPlatforms);
	printf("\nNumber of platforms:\t%u\n\n", numOfPlatforms);

	for(unsigned int i = 0; i < numOfPlatforms; i++)
	{
		printf("Platform:\t\t%u\n\n", i);
		printPlatformInfo(platformIds[i], indent);

		clGetDeviceIDs (platformIds[i], CL_DEVICE_TYPE_ALL, sizeof(devices), devices, &numOfDevices);
		printf("\tNumber of devices:\t%u\n\n", numOfDevices);

		for(unsigned int j = 0; j < numOfDevices; j++)
		{
			printf("\tDevice: %u\n", j);
			printDeviceInfo(devices[j], indent+1);
		}
	}
	printf("\n====================================================================\n\n");
}

void OpenCLUtilities::printPlatformInfo(cl_platform_id platform, unsigned int indentTimes)
{
	char platformVendor[MAX_CHAR_LENGTH];
	char platformName[MAX_CHAR_LENGTH];
	char platformExtensions[MAX_CHAR_LENGTH];
	char platformVersion[MAX_CHAR_LENGTH];
	char platformProfile[MAX_CHAR_LENGTH];

	clGetPlatformInfo (platform, CL_PLATFORM_VENDOR, sizeof(platformVendor), &platformVendor, NULL);
	clGetPlatformInfo (platform, CL_PLATFORM_NAME, sizeof(platformName), &platformName, NULL);
	clGetPlatformInfo (platform, CL_PLATFORM_EXTENSIONS, sizeof(platformExtensions), &platformExtensions, NULL);
	clGetPlatformInfo (platform, CL_PLATFORM_VERSION, sizeof(platformVersion), &platformVersion, NULL);
	clGetPlatformInfo (platform, CL_PLATFORM_PROFILE, sizeof(platformProfile), &platformProfile, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- PlatformInfo ---\n", indent);
	printf("%sName:\t\t\t%s\n", indent, platformName);
	printf("%sVendor:\t\t\t%s\n", indent, platformVendor);
	printf("%sVersion:\t\t%s\n", indent, platformVersion);
	printf("%sProfil:\t\t\t%s\n", indent, platformProfile);
	printf("%sExtensions:\n", indent);
	printCharList(platformExtensions, " ", getIndentation(indentTimes+1).c_str());

	printf("\n");
}

void OpenCLUtilities::printDeviceInfo(cl_device_id device, unsigned int indentTimes)
{
	char driverVersion[MAX_CHAR_LENGTH];				//OpenCL software driver version string
	char deviceVendor[MAX_CHAR_LENGTH];					//this strirng will hold a platforms vendor
	cl_uint deviceVendorId;								//A unique device vendor identifier.
	char deviceVersion[MAX_CHAR_LENGTH];				//OpenCL version supported by the device
	char deviceName[MAX_CHAR_LENGTH];					//this string will hold the devices name
	cl_uint numberOfCores;								//this variable holds the number of cores of on a device
	cl_long amountOfMemory;								//this variable holds the amount of memory on a device
	cl_uint clockFreq;									//this variable holds the clock frequency of a device
	cl_ulong maxAlocatableMem;							//this variable holds the maximum allocatable memory
	cl_ulong localMem;									//this variable holds local memory for a device
	cl_bool	available;									//this variable holds if the device is available
	size_t maxWorkGroupSize;							//Maximum number of work-items in a work-group executing a kernel using the data parallel execution model.
	cl_uint maxWorkItemDimensions;						//Maximum dimensions that specify the global and local work-item IDs used by the data parallel execution model. (Refer to clEnqueueNDRangeKernel). The minimum value is 3.
	size_t maxWorkItemSize[MAX_WORK_ITEM_DIMENSIONS];	//Maximum number of work-items that can be specified in each dimension of the work-group to clEnqueueNDRangeKernel
	cl_bool imageSupport;								//Is CL_TRUE if images are supported by the OpenCL device and CL_FALSE otherwise.
	size_t image3DMaxHeight;							//Max height of 3D image in pixels.
	size_t image3DMaxWidth;								//Max width of 3D image in pixels.
	size_t image3DMaxDepth;								//Max depth of 3D image in pixels.
	size_t image2DMaxHeight;							//Max height of 2D image in pixels.
	size_t image2DMaxWidth;								//Max width of 2D image in pixels.
	cl_bool errorCorrectionSupport;						//Is CL_TRUE if the device implements error correction for the memories, caches, registers etc. in the device. Is CL_FALSE if the device does not implement error correction. This can be a requirement for certain clients of OpenCL.
	size_t profilingTimerResolution;					//Describes the resolution of device timer. This is measured in nanoseconds.
	cl_bool endianLittle;								//Is CL_TRUE if the OpenCL device is a little endian device and CL_FALSE otherwise.
	char deviceProfile[MAX_CHAR_LENGTH];				//OpenCL profile string. Returns the profile name supported by the device.
	char deviceExtensions[MAX_CHAR_LENGTH];				//Returns a space separated list of extension names


	clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceName), &deviceName, NULL);
	clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(deviceVendor), &deviceVendor, NULL);
	clGetDeviceInfo(device, CL_DEVICE_VENDOR_ID, sizeof(deviceVendorId), &deviceVendorId, NULL);
	clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(deviceVersion), &deviceVersion, NULL);
	clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(driverVersion), &driverVersion, NULL);
	clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(numberOfCores), &numberOfCores, NULL);
	clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(amountOfMemory), &amountOfMemory, NULL);
	clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clockFreq), &clockFreq, NULL);
	clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxAlocatableMem), &maxAlocatableMem, NULL);
	clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMem), &localMem, NULL);
	clGetDeviceInfo(device, CL_DEVICE_AVAILABLE, sizeof(available), &available, NULL);
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxWorkItemDimensions), &maxWorkItemDimensions, NULL);
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItemSize), &maxWorkItemSize, NULL);
	clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(imageSupport), &imageSupport, NULL);
	clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(image2DMaxWidth), &image2DMaxWidth, NULL);
	clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(image2DMaxHeight), &image2DMaxHeight, NULL);
	clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(image3DMaxWidth), &image3DMaxWidth, NULL);
	clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(image3DMaxHeight), &image3DMaxHeight, NULL);
	clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(image3DMaxDepth), &image3DMaxDepth, NULL);
	clGetDeviceInfo(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(errorCorrectionSupport), &errorCorrectionSupport, NULL);
	clGetDeviceInfo(device, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(profilingTimerResolution), &profilingTimerResolution, NULL);
	clGetDeviceInfo(device, CL_DEVICE_ENDIAN_LITTLE, sizeof(endianLittle), &endianLittle, NULL);
	clGetDeviceInfo(device, CL_DEVICE_PROFILE, sizeof(deviceProfile), &deviceProfile, NULL);
	clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(deviceExtensions), &deviceExtensions, NULL);


	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- DeviceInfo ---\n", indent);
	printf("%sName:\t\t\t\t%s\n", indent, deviceName);
	printf("%sVendor:\t\t\t\t%s\n", indent, deviceVendor);
	printf("%sVendor id:\t\t\t%u\n", indent, deviceVendorId);
	printf("%sDevice supports:\t\t%s \n", indent, deviceVersion);
	printf("%sOpenCL driver version:\t\t%s\n", indent, driverVersion);
	printf("%sDevice Profile:\t\t\t%s \n", indent, deviceProfile);
	printf("%sAvailable:\t\t\t%s\n", indent, available ? "Yes" : "No");
	printf("\n");

	printf("%sCompute Units:\t\t\t%u\n", indent, numberOfCores);
	printf("%sClock Frequency:\t\t%u mHz\n", indent, clockFreq);
	printf("%sGlobal Memory:\t\t\t%0.00f mb\n", indent, (double)amountOfMemory/1048576);
	printf("%sMax Allocateable Memory:\t%0.00f mb\n", indent, (double)maxAlocatableMem/1048576);
	printf("%sLocal Memory:\t\t\t%u kb\n", indent, (unsigned int)localMem);
	printf("\n");

	printf("%sError correction support:\t%s\n", indent, errorCorrectionSupport ? "Yes" : "No");
	printf("%sProfiling Timer Resolution:\t%u ns\n", indent, (unsigned int)profilingTimerResolution);
	printf("%sEndian Little:\t\t\t%s\n", indent, endianLittle ? "Yes" : "No");
	printf("\n");

	printf("%sMax work group size:\t\t%u\n", indent, (unsigned int)maxWorkGroupSize);
	printf("%sMax work item dimensions:\t%u\n", indent, maxWorkItemDimensions);
	for(int k = 0; k < maxWorkItemDimensions; k++)
	{
		printf("%s\tDimension %u supports maximum %u work items\n", indent, k, (unsigned int)maxWorkItemSize[k]);
	}
	printf("\n");

	printf("%sImage support:\t\t\t%s\n", indent, imageSupport ? "Yes" : "No");
	printf("%sImage2D max width:\t\t%u\n", indent, (unsigned int)image2DMaxWidth);
	printf("%sImage2D max height:\t\t%u\n", indent, (unsigned int)image2DMaxHeight);
	printf("%sImage3D max width:\t\t%u\n", indent, (unsigned int)image3DMaxWidth);
	printf("%sImage3D max height:\t\t%u\n", indent, (unsigned int)image3DMaxHeight);
	printf("%sImage3D max depth:\t\t%u\n", indent, (unsigned int)image3DMaxWidth);

	printf("\n");

	printf("%sExtensions:\n", indent);
	printCharList(deviceExtensions, " ", getIndentation(indentTimes+1).c_str());

	printf("\n");
}

void OpenCLUtilities::printContextInfo(cl_context context, unsigned int indentTimes)
{
	cl_uint referenceCount;
	cl_uint numberOfDevicesInContext;
	cl_device_id listOfDevicesInContext[MAX_NUMBER_OF_DEVICES];
	cl_context_properties properties[MAX_NUMBER_OF_PROPERITES];

	clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(referenceCount), &referenceCount, NULL);
	clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(numberOfDevicesInContext), &numberOfDevicesInContext, NULL);
	clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(listOfDevicesInContext), &listOfDevicesInContext, NULL);
	//clGetContextInfo(context, CL_CONTEXT_PROPERTIES, sizeof(properties), &properties, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- ContextInfo ---\n", indent);
	printf("%sReference count:\t%u\n", indent, referenceCount);
	printf("%sDevices in context:\t%u\n", indent, numberOfDevicesInContext);
	for(unsigned int i=0; i<numberOfDevicesInContext; i++)
	{
		printf("%s\tDevice %us id:\t%u\n", indent, i, listOfDevicesInContext[i]);
	}
}

void OpenCLUtilities::printProgramInfo(cl_program program, unsigned int indentTimes, bool printSource)
{
	cl_uint referenceCount;
	cl_context context;
	cl_uint numberOfAssociatedDevices;
	cl_device_id devices[MAX_NUMBER_OF_DEVICES];
	char source[MAX_SOURCE_LENGTH];
	size_t binarySizes[MAX_NUMBER_OF_DEVICES];

	clGetProgramInfo(program, CL_PROGRAM_REFERENCE_COUNT, sizeof(referenceCount), &referenceCount, NULL);
	clGetProgramInfo(program, CL_PROGRAM_CONTEXT, sizeof(context), &context, NULL);
	clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(numberOfAssociatedDevices), &numberOfAssociatedDevices, NULL);
	clGetProgramInfo(program, CL_PROGRAM_DEVICES, sizeof(devices), &devices, NULL);
	clGetProgramInfo(program, CL_PROGRAM_SOURCE, sizeof(source), &source, NULL);
	clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(binarySizes), &binarySizes, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- ProgramInfo ---\n", indent);
	printf("%sReference count:\t%u\n", indent, referenceCount);
	printf("%sAssociated devices:\t%u\n", indent, numberOfAssociatedDevices);
	for(unsigned int i=0; i<numberOfAssociatedDevices; i++)
	{
		printf("%s\tDevice %us id:\t%u\n", indent, i, devices[i]);
	}
	if(printSource)
		printf("%sProgram source:\n%s\n", indent, source);
	printf("%sBinary size:\t%u\n", indent, binarySizes);
	printContextInfo(context,indentTimes+1);

}

void OpenCLUtilities::printKernelInfo(cl_kernel kernel, unsigned int indentTimes)
{
	char kernelFunctionName[MAX_CHAR_LENGTH];
	cl_uint numberOfKernelArgs;
	cl_uint referenceCount;
	cl_context context;
	cl_program program;

	clGetKernelInfo(kernel, CL_KERNEL_FUNCTION_NAME, sizeof(kernelFunctionName), &kernelFunctionName, NULL);
	clGetKernelInfo(kernel, CL_KERNEL_NUM_ARGS, sizeof(numberOfKernelArgs), &numberOfKernelArgs, NULL);
	clGetKernelInfo(kernel, CL_KERNEL_REFERENCE_COUNT, sizeof(referenceCount), &referenceCount, NULL);
	clGetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(context), &context, NULL);
	clGetKernelInfo(kernel, CL_KERNEL_PROGRAM, sizeof(program), &program, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- KernelInfo ---\n", indent);
	printf("%sFunction name:\t\t%s\n", indent,  kernelFunctionName);
	printf("%sNumber of args:\t\t%u\n", indent, numberOfKernelArgs);
	printf("%sReference count:\t%u\n", indent, referenceCount);
	printContextInfo(context, indentTimes+1);
	printProgramInfo(program, indentTimes+1);

}

void OpenCLUtilities::printCharList(const char* list, const char* separator, const char* indentation)
{
	std::string stdString(list);
	std::vector<std::string> strings;
	boost::split(strings, stdString, boost::is_any_of(std::string(separator)));
	std::vector<std::string>::iterator it = strings.begin();
	for(it; it != strings.end(); ++it)
		printf("%s%s\n", indentation, (*it).c_str());
}

std::string OpenCLUtilities::getIndentation(unsigned int numberOfIndents)
{
	std::string retval("");
	for(unsigned int i=0; i < numberOfIndents; i++)
		retval += DEFAULT_INDENTATION;
	return retval;
}
}//namespace cx


#endif //SSC_USE_OpenCL

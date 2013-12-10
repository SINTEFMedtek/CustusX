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

void OpenCLInfo::printPlatformAndDeviceInfo()
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
			printDeviceInfo(devices[j], indent+1, true);
		}
	}
	printf("\n====================================================================\n\n");
}

void OpenCLInfo::printPlatformInfo(cl_platform_id platform, unsigned int indentTimes)
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

void OpenCLInfo::printDeviceInfo(cl_device_id device, unsigned int indentTimes, bool verbose)
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
	cl_command_queue_properties commandQueueProps;
	char openClCVersion[MAX_CHAR_LENGTH];
	cl_device_exec_capabilities	exeCapabilities;
	cl_bool hostUnifiedMemory;


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
	clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(commandQueueProps), &commandQueueProps, NULL);
	clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, sizeof(openClCVersion), &openClCVersion, NULL);
	clGetDeviceInfo(device, CL_DEVICE_EXECUTION_CAPABILITIES, sizeof(exeCapabilities), &exeCapabilities, NULL);
	clGetDeviceInfo(device, CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(hostUnifiedMemory), &hostUnifiedMemory, NULL);


	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- DeviceInfo ---\n", indent);
	printf("%sName:\t\t\t\t%s\n", indent, deviceName);
	printf("%sVendor:\t\t\t\t%s\n", indent, deviceVendor);
	printf("%sVendor id:\t\t\t%u\n", indent, deviceVendorId);
	printf("%sDevice supports:\t\t%s \n", indent, deviceVersion);
	printf("%sOpenCL driver version:\t\t%s\n", indent, driverVersion);
	printf("%sHighest version can compile:\t%s\n", indent, openClCVersion);
	printf("%sAvailable:\t\t\t%s\n", indent, available ? "Yes" : "No");
	printf("\n");

	if(!verbose)
		return;

	//Verbose output
	printf("%sCompute Units:\t\t\t%u\n", indent, numberOfCores);
	printf("%sClock Frequency:\t\t%u mHz\n", indent, clockFreq);
	printf("%sGlobal Memory:\t\t\t%0.00f mb\n", indent, (double)amountOfMemory/1048576);
	printf("%sMax Allocateable Memory:\t%0.00f mb\n", indent, (double)maxAlocatableMem/1048576);
	printf("%sLocal Memory:\t\t\t%llu kb\n", indent, localMem);
	printf("\n");

	printf("%sDevice Profile:\t\t\t%s \n", indent, deviceProfile);
	printf("%sError correction support:\t%s\n", indent, errorCorrectionSupport ? "Yes" : "No");
	printf("%sProfiling Timer Resolution:\t%lu ns\n", indent, profilingTimerResolution);
	printf("%sEndian Little:\t\t\t%s\n", indent, endianLittle ? "Yes" : "No");
	printf("%sCommand queue properties:\t%llu\n", indent, commandQueueProps);
	printf("%sExecution capabilities:\t\t%llu\n", indent, exeCapabilities);
	printf("%sHost unified memory:\t\t%s\n", indent, hostUnifiedMemory ? "Yes" : "No");
	printf("\n");

	printf("%sMax work group size:\t\t%lu\n", indent, maxWorkGroupSize);
	printf("%sMax work item dimensions:\t%u\n", indent, maxWorkItemDimensions);
	for(int k = 0; k < maxWorkItemDimensions; k++)
	{
		printf("%s\tDimension %u supports maximum %lu work items\n", indent, k, maxWorkItemSize[k]);
	}
	printf("\n");

	printf("%sImage support:\t\t\t%s\n", indent, imageSupport ? "Yes" : "No");
	printf("%sImage2D max width:\t\t%lu\n", indent, image2DMaxWidth);
	printf("%sImage2D max height:\t\t%lu\n", indent,image2DMaxHeight);
	printf("%sImage3D max width:\t\t%lu\n", indent, image3DMaxWidth);
	printf("%sImage3D max height:\t\t%lu\n", indent, image3DMaxHeight);
	printf("%sImage3D max depth:\t\t%lu\n", indent, image3DMaxWidth);

	printf("\n");

	printf("%sExtensions:\n", indent);
	printCharList(deviceExtensions, " ", getIndentation(indentTimes+1).c_str());
}

void OpenCLInfo::printContextInfo(cl_context context, unsigned int indentTimes)
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
		printDeviceInfo(listOfDevicesInContext[i], indentTimes+1);
}

void OpenCLInfo::printProgramInfo(cl_program program, unsigned int indentTimes, bool printSource)
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
		printf("%s - Binary size:\t\t%lu bytes\n", indent, binarySizes[i]);
		printDeviceInfo(devices[i], indentTimes+1);
	}
	if(printSource)
		printf("%sProgram source:\n%s\n", indent, source);
	printContextInfo(context,indentTimes+1);

}

void OpenCLInfo::printProgramBuildInfo(cl_program program, cl_device_id device, unsigned int indentTimes)
{
	cl_build_status status;
	char buildOptions[MAX_CHAR_LENGTH];
	char buildLog[MAX_SOURCE_LENGTH];

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS, sizeof(status), &status, NULL);
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_OPTIONS, sizeof(buildOptions), &buildOptions, NULL);
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), &buildLog, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- ProgramBuildInfo ---\n", indent);
	printf("%sStatus:\t\t%u\n", indent,  status);
	printf("%sBuild options:\t\t%s\n", indent, buildOptions);
	printf("%sBuild log:\t%s\n", indent, buildLog);
}

void OpenCLInfo::printKernelInfo(cl_kernel kernel, unsigned int indentTimes)
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

void OpenCLInfo::printKernelWorkGroupInfo(cl_kernel kernel, cl_device_id device, unsigned int indentTimes)
{
	size_t workGroupSize;
	size_t compileWorkGroupSize[3];
	cl_ulong localMemSize;
	size_t preferredWorkGroupSizeMultiple;
	cl_ulong privateMemSize;

	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(workGroupSize), &workGroupSize, NULL);
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(compileWorkGroupSize), &compileWorkGroupSize, NULL);
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(localMemSize), &localMemSize, NULL);
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(preferredWorkGroupSizeMultiple), &preferredWorkGroupSizeMultiple, NULL);
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(privateMemSize), &privateMemSize, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- KernelWorkGroupInfo ---\n", indent);
	printf("%sWork group size:\t\t%lu\n", indent,  workGroupSize);
	printf("%sCompiler work group size:\t(%lu,%lu,%lu)\n", indent, compileWorkGroupSize[0], compileWorkGroupSize[1], compileWorkGroupSize[2]);
	printf("%sLocal mem size:\t\t\t%llu\n", indent, localMemSize);
	printf("%sPreferred work group size:\t%lu\n", indent, preferredWorkGroupSizeMultiple);
	printf("%sPrivate mem size:\t\t%llu\n", indent, privateMemSize);
}

void OpenCLInfo::printCommandQueueInfo(cl_command_queue command_queue, unsigned int indentTimes)
{
	cl_context context;
	cl_device_id deviceId;
	cl_uint referenceCount;
	cl_command_queue_properties properties;

	clGetCommandQueueInfo(command_queue, CL_QUEUE_CONTEXT, sizeof(context), &context, NULL);
	clGetCommandQueueInfo(command_queue, CL_QUEUE_DEVICE, sizeof(deviceId), &deviceId, NULL);
	clGetCommandQueueInfo(command_queue, CL_QUEUE_REFERENCE_COUNT, sizeof(referenceCount), &referenceCount, NULL);
	clGetCommandQueueInfo(command_queue, CL_QUEUE_PROPERTIES, sizeof(properties), &properties, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- CommandQueueInfo ---\n", indent);
	printf("%sReference count:\t%u\n", indent, referenceCount);
	printContextInfo(context, indentTimes+1);
	printDeviceInfo(deviceId, indentTimes+1);
}

void OpenCLInfo::printMemInfo(cl_mem memobj, unsigned int indentTimes)
{
	if(memobj == NULL)
		return;

	cl_mem_object_type type;
	cl_mem_flags flags;
	size_t size;
	void *host;
	cl_uint mapCount;
	cl_uint referenceCount;
	cl_context context;
	cl_mem associatedMemObject;
	size_t offset;

	clGetMemObjectInfo(memobj, CL_MEM_TYPE, sizeof(type), &type, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_FLAGS, sizeof(flags), &flags, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_SIZE, sizeof(size), &size, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_HOST_PTR, sizeof(host), &host, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_MAP_COUNT, sizeof(mapCount), &mapCount, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_REFERENCE_COUNT, sizeof(referenceCount), &referenceCount, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_CONTEXT, sizeof(context), &context, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_ASSOCIATED_MEMOBJECT, sizeof(associatedMemObject), &associatedMemObject, NULL);
	clGetMemObjectInfo(memobj, CL_MEM_OFFSET, sizeof(offset), &offset, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- MemObjInfo ---\n", indent);
	printf("%sType:\t\t\t%u\n", indent, type);
	printf("%sFlags:\t\t\t%llu\n", indent, flags);
	printf("%sSize:\t\t\t%lu bytes\n", indent, size);
	printf("%sMap count:\t\t%u\n", indent, mapCount);
	printf("%sOffset:\t\t\t%lu\n", indent, offset);
	printf("%sReference count:\t%u\n", indent, referenceCount);
	printContextInfo(context, indentTimes+1);
	printMemInfo(associatedMemObject, indentTimes+1);
}

void OpenCLInfo::printSamplerInfo(cl_sampler sampler, unsigned int indentTimes)
{
	cl_uint referenceCount;
	cl_context context;
	cl_bool normalizedCoords;
	cl_addressing_mode addressingMode;
	cl_filter_mode filterMode;

	clGetSamplerInfo(sampler, CL_SAMPLER_REFERENCE_COUNT, sizeof(referenceCount), &referenceCount, NULL);
	clGetSamplerInfo(sampler, CL_SAMPLER_CONTEXT, sizeof(context), &context, NULL);
	clGetSamplerInfo(sampler, CL_SAMPLER_NORMALIZED_COORDS, sizeof(normalizedCoords), &normalizedCoords, NULL);
	clGetSamplerInfo(sampler, CL_SAMPLER_ADDRESSING_MODE, sizeof(addressingMode), &addressingMode, NULL);
	clGetSamplerInfo(sampler, CL_SAMPLER_FILTER_MODE, sizeof(filterMode), &filterMode, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- SamplerInfo ---\n", indent);
	printf("%sNormalized coords:\t\t\t%s\n", indent, normalizedCoords ? "Yes" : "No");
	printf("%sAddressing mode:\t\t\t%u\n", indent, addressingMode);
	printf("%sFilter mode:\t\t\t%u\n", indent, filterMode);
	printf("%sReference count:\t%u\n", indent, referenceCount);
	printContextInfo(context, indentTimes+1);
}

void OpenCLInfo::printEventInfo(cl_event event, unsigned int indentTimes)
{
	cl_command_queue commandQueue;
	cl_context context;
	cl_command_type type;
	cl_int	executionStatus;
	cl_uint referenceCount;

	clGetEventInfo(event, CL_EVENT_COMMAND_QUEUE, sizeof(commandQueue), &commandQueue, NULL);
	clGetEventInfo(event, CL_EVENT_CONTEXT, sizeof(context), &context, NULL);
	clGetEventInfo(event, CL_EVENT_COMMAND_TYPE, sizeof(type), &type, NULL);
	clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(executionStatus), &executionStatus, NULL);
	clGetEventInfo(event, CL_EVENT_REFERENCE_COUNT, sizeof(referenceCount), &referenceCount, NULL);

	const char* indent = getIndentation(indentTimes).c_str();
	printf("%s--- EventInfo ---\n", indent);
	printf("%sType:\t\t\t%u\n", indent, type);
	printf("%sExecution status:\t\t\t%u\n", indent, executionStatus);
	printf("%sReference count:\t%u\n", indent, referenceCount);
	printCommandQueueInfo(commandQueue);
	printContextInfo(context, indentTimes+1);
}

void OpenCLInfo::printCharList(const char* list, const char* separator, const char* indentation)
{
	std::string stdString(list);
	std::vector<std::string> strings;
	boost::split(strings, stdString, boost::is_any_of(std::string(separator)));
	std::vector<std::string>::iterator it = strings.begin();
	for(it; it != strings.end(); ++it)
		printf("%s%s\n", indentation, (*it).c_str());
}

std::string OpenCLInfo::getIndentation(unsigned int numberOfIndents)
{
	std::string retval("");
	for(unsigned int i=0; i < numberOfIndents; i++)
		retval += DEFAULT_INDENTATION;
	return retval;
}
}//namespace cx


#endif //SSC_USE_OpenCL

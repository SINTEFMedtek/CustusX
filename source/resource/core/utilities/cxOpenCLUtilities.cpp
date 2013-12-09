#ifdef SSC_USE_OpenCL

#include "cxOpenCLUtilities.h"
#include <stdio.h>

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__


namespace cx
{

OpenCLUtilities::OpenCLUtilities()
{}

void OpenCLUtilities::printPlatformAndDeviceInfo()
{
	//Based on:
	//https://devtalk.nvidia.com/default/topic/498968/printing-all-opencl-devices-not-detecting-all-opencl-devices-under-windows/

	unsigned int i, j;				//iterator variables for loops

	cl_platform_id platforms[32];		//an array to hold the IDs of all the platforms, hopefuly there won't be more than 32
	cl_uint num_platforms;				//this number will hold the number of platforms on this machine
	char vendor[1024];					//this strirng will hold a platforms vendor
	char driverVersion[1024];			//OpenCL software driver version string
	char deviceVersion[1024];			//OpenCL version supported by the device
	cl_device_id devices[32];			//this variable holds the number of devices for each platform, hopefully it won't be more than 32 per platform
	cl_uint num_devices;				//this number will hold the number of devices on this machine
	char deviceName[1024];				//this string will hold the devices name
	cl_uint numberOfCores;				//this variable holds the number of cores of on a device
	cl_long amountOfMemory;				//this variable holds the amount of memory on a device
	cl_uint clockFreq;					//this variable holds the clock frequency of a device
	cl_ulong maxAlocatableMem;			//this variable holds the maximum allocatable memory
	cl_ulong localMem;					//this variable holds local memory for a device
	cl_bool	available;					//this variable holds if the device is available
	size_t maxWorkGroupSize;			//Maximum number of work-items in a work-group executing a kernel using the data parallel execution model.
	cl_uint maxWorkItemDimensions;		//Maximum dimensions that specify the global and local work-item IDs used by the data parallel execution model. (Refer to clEnqueueNDRangeKernel). The minimum value is 3.
	size_t maxWorkItemSize[1024];		//Maximum number of work-items that can be specified in each dimension of the work-group to clEnqueueNDRangeKernel
	cl_bool imageSupport;				//Is CL_TRUE if images are supported by the OpenCL device and CL_FALSE otherwise.
	size_t image3DMaxHeight;			//Max height of 3D image in pixels.
	size_t image3DMaxWidth;				//Max width of 3D image in pixels.
	size_t image3DMaxDepth;				//Max depth of 3D image in pixels.
	size_t image2DMaxHeight;			//Max height of 2D image in pixels.
	size_t image2DMaxWidth;				//Max width of 2D image in pixels.
	cl_bool errorCorrectionSupport;		//Is CL_TRUE if the device implements error correction for the memories, caches, registers etc. in the device. Is CL_FALSE if the device does not implement error correction. This can be a requirement for certain clients of OpenCL.
	size_t profilingTimerResolution;	//Describes the resolution of device timer. This is measured in nanoseconds.
	cl_bool endianLittle;				//Is CL_TRUE if the OpenCL device is a little endian device and CL_FALSE otherwise.
	char deviceProfile[1024];			//OpenCL profile string. Returns the profile name supported by the device.

	//get the number of platforms
	clGetPlatformIDs (32, platforms, &num_platforms);
	printf("\nNumber of platforms:\t%u\n\n", num_platforms);

	//this is a loop for platforms
	for(i = 0; i < num_platforms; i++)
	{
		printf("Platform:\t\t%u\n\n", i);

		clGetPlatformInfo (platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, NULL);
		printf("\tPlatform Vendor:\t%s\n", vendor);

		clGetDeviceIDs (platforms[i], CL_DEVICE_TYPE_ALL, sizeof(devices), devices, &num_devices);
		printf("\tNumber of devices:\t%u\n\n", num_devices);

		//this is a loop for devices
		for(j = 0; j < num_devices; j++)
		{
			//scan in device information
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, sizeof(deviceVersion), &deviceVersion, NULL);
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, sizeof(driverVersion), &driverVersion, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(numberOfCores), &numberOfCores, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(amountOfMemory), &amountOfMemory, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clockFreq), &clockFreq, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxAlocatableMem), &maxAlocatableMem, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMem), &localMem, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, sizeof(available), &available, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxWorkItemDimensions), &maxWorkItemDimensions, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItemSize), &maxWorkItemSize, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE_SUPPORT, sizeof(imageSupport), &imageSupport, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(image2DMaxWidth), &image2DMaxWidth, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(image2DMaxHeight), &image2DMaxHeight, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(image3DMaxWidth), &image3DMaxWidth, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(image3DMaxHeight), &image3DMaxHeight, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(image3DMaxDepth), &image3DMaxDepth, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(errorCorrectionSupport), &errorCorrectionSupport, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(profilingTimerResolution), &profilingTimerResolution, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_ENDIAN_LITTLE, sizeof(endianLittle), &endianLittle, NULL);
			clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, sizeof(deviceProfile), &deviceProfile, NULL);


			//print out device information
			printf("\tDevice: %u\n", j);
			printf("\t\tName:\t\t\t\t%s\n", deviceName);
			printf("\t\tVendor:\t\t\t\t%s\n", vendor);
			printf("\t\tDevice supports:\t\t%s \n", deviceVersion);
			printf("\t\tOpenCL driver version:\t\t%s \n", driverVersion);
			printf("\t\tDevice Profile:\t\t\t%s \n", deviceProfile);
			printf("\t\tAvailable:\t\t\t%s\n", available ? "Yes" : "No");
			printf("\n");

			printf("\t\tCompute Units:\t\t\t%u\n", numberOfCores);
			printf("\t\tClock Frequency:\t\t%u mHz\n", clockFreq);
			printf("\t\tGlobal Memory:\t\t\t%0.00f mb\n", (double)amountOfMemory/1048576);
			printf("\t\tMax Allocateable Memory:\t%0.00f mb\n", (double)maxAlocatableMem/1048576);
			printf("\t\tLocal Memory:\t\t\t%u kb\n", (unsigned int)localMem);
			printf("\n");

			printf("\t\tError correction support:\t%s\n", errorCorrectionSupport ? "Yes" : "No");
			printf("\t\tProfiling Timer Resolution:\t%u ns\n", profilingTimerResolution);
			printf("\t\tEndian Little:\t\t\t%s\n", endianLittle ? "Yes" : "No");
			printf("\n");

			printf("\t\tMax work group size:\t\t%u\n", (unsigned int)maxWorkGroupSize);
			printf("\t\tMax work item dimensions:\t%u\n", maxWorkItemDimensions);
			for(int k = 0; k < maxWorkItemDimensions; k++)
			{
				printf("\t\t\tDimension %u supports maximum %u work items\n", k, (unsigned int)maxWorkItemSize[k]);
			}
			printf("\n");

			printf("\t\tImage support:\t\t\t%s\n", imageSupport ? "Yes" : "No");
			printf("\t\tImage2D max width:\t\t%u\n", (unsigned int)image2DMaxWidth);
			printf("\t\tImage2D max height:\t\t%u\n", (unsigned int)image2DMaxHeight);
			printf("\t\tImage3D max width:\t\t%u\n", (unsigned int)image3DMaxWidth);
			printf("\t\tImage3D max height:\t\t%u\n", (unsigned int)image3DMaxHeight);
			printf("\t\tImage3D max depth:\t\t%u\n", (unsigned int)image3DMaxWidth);

			printf("\n");
		}

	}
}

}//namespace cx


#endif //SSC_USE_OpenCL

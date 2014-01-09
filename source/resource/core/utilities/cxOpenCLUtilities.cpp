#ifdef SSC_USE_OpenCL

#include "cxOpenCLUtilities.h"

#include <iostream>
#include <QString>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"

#include "cxOpenCLPrinter.h"

namespace cx
{

void CL_CALLBACK errorCallback(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	messageManager()->sendError("Error callback: " + QString(errinfo));
}

void CL_CALLBACK memoryDestructorCallback(cl_mem memobj, void* user_data)
{
	std::string* data_pointer = static_cast<std::string*>(user_data);
	std::string data = *data_pointer;
	messageManager()->sendInfo("Memory destructor callback: " + QString(data.c_str()));
}

OpenCL::ocl* OpenCL::init(cl_device_type type)
{
	cl::Platform platform = selectPlatform();
    cl::Device device = selectDevice(type, platform);
	cl_context_properties cps[] = { CL_CONTEXT_PLATFORM, (cl_context_properties) (platform)(), 0 };
    cl::Context context = createContext(device, cps);
    cl::CommandQueue commandQueue = createCommandQueue(context, device);

    //fill the struct
	OpenCL::ocl* retval = new OpenCL::ocl;
    retval->device_cpp = device;
    retval->context_cpp = context;
    retval->cmd_queue_cpp = commandQueue;

    return retval;
}

cl::Platform OpenCL::selectPlatform()
{
	cl::Platform retval;
    try
	{
		VECTOR_CLASS<cl::Platform> platforms;
    	cl::Platform::get(&platforms);

    	int foundPlatforms = platforms.size();
    	if(foundPlatforms == 0)
    		throw cl::Error(1, "No OpenCL platforms found.");
    	else if(foundPlatforms > 1)
        	messageManager()->sendWarning("The number of platforms found differs from 1. This might not be supported.");

        int platformToSelect = 0; //just select the first platform in the list
        retval = platforms[platformToSelect];

	}catch(cl::Error error)
	{
		messageManager()->sendError("Could not select a OpenCL platform. Reason: "+QString(error.what()));
		check_error(error.err());
	}

	std::string name, vendor, version;
	name = retval.getInfo<CL_PLATFORM_NAME>();
	vendor = retval.getInfo<CL_PLATFORM_VENDOR>();
	version = retval.getInfo<CL_PLATFORM_VERSION>();
	messageManager()->sendInfo("Selected platform "+qstring_cast(name)+" from vendor "+qstring_cast(vendor)+" using "+qstring_cast(version));

	return retval;
}

cl::Device OpenCL::selectDevice(cl_device_type type, cl::Platform platform)
{
	cl::Device retval;

	std::string typeString  = "";
	switch (type)
	{
		case CL_DEVICE_TYPE_CPU: typeString = "CPU"; break;
		case CL_DEVICE_TYPE_GPU: typeString = "GPU"; break;
		case CL_DEVICE_TYPE_ALL: typeString = "ALL"; break;
		default: break;
	}

	try
	{
		VECTOR_CLASS<cl::Device> devices;
		platform.getDevices(type, &devices);

		int foundDevices = devices.size();
		messageManager()->sendDebug("Found "+QString::number(foundDevices)+" devices.");
		if(foundDevices == 0)
			throw cl::Error(1, std::string("No OpenCL devices of type "+typeString+" found on this platform.").c_str());

		retval = chooseDeviceWithMostGlobalMemory(devices);

	}catch(cl::Error error)
	{
		messageManager()->sendError("Could not select a OpenCL device. Reason: "+QString(error.what()));
		check_error(error.err());
	}

	cl::STRING_CLASS name, vendor, version;
	name = retval.getInfo<CL_DEVICE_NAME>();
	vendor = retval.getInfo<CL_DEVICE_VENDOR>();
	version = retval.getInfo<CL_DEVICE_VERSION>();
	messageManager()->sendInfo("Selected device "+qstring_cast(name)+" from vendor "+qstring_cast(vendor)+" using "+qstring_cast(version));

    return retval;

}

cl::Device OpenCL::chooseDeviceWithMostGlobalMemory(VECTOR_CLASS<cl::Device> devices)
{
	if(devices.size() <= 0)
		throw cl::Error(1, "Cannot choose among 0 devices.");

	cl::Device retval = devices[0];

	cl_ulong largestMemory = 0;
	cl_ulong deviceMemory = 0;
	for(int i=0; i < devices.size(); i++)
	{
		try
		{
			deviceMemory = devices[i].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
			if(deviceMemory > largestMemory)
			{
				largestMemory = deviceMemory;
				retval = devices[i];
			}
		} catch (cl::Error error)
		{
			messageManager()->sendWarning("Could not ask device about CL_DEVICE_GLOBAL_MEM_SIZE. Reason: "+QString(error.what()));
			check_error(error.err());
		}
	}
	cl::STRING_CLASS name;
	name = retval.getInfo<CL_DEVICE_NAME>();
	messageManager()->sendInfo("Device with most global memory is "+qstring_cast(name)+" with "+QString::number((double)largestMemory/(1024*1024*1024))+" GB.");

	return retval;
}

cl::Context OpenCL::createContext(cl::Device device, cl_context_properties* cps)
{
	VECTOR_CLASS<cl::Device> devices;
	devices.push_back(device); //if using std vector for VECTOR_CLASS this results in size = 2 instead of the correct 1.
	return createContext(devices, cps);
}

cl::Context OpenCL::createContext(const VECTOR_CLASS<cl::Device> devices, cl_context_properties* cps)
{
	cl::Context retval;
	try
	{
		messageManager()->sendDebug("Going to create a OpenCL context with "+QString::number(devices.size())+" device(s).");
		//retval = cl::Context(devices, cps, errorCallback, NULL, NULL);
		//TODO if extension is available use:
		//retval = cl::Context(devices, cps, clLogMessagesToStdoutAPPLEretval, NULL, NULL);
		retval = cl::Context(devices, cps, errorCallback, NULL, NULL);
		cl::STRING_CLASS devicelist="";
		for(int i=0; i<devices.size(); ++i)
		{
			devicelist = devicelist + devices[i].getInfo<CL_DEVICE_NAME>();
			if(i != devices.size()-1)
				devicelist =+ " and ";
		}
		messageManager()->sendInfo("Created context using device(s) "+qstring_cast(devicelist));

		VECTOR_CLASS<cl::Device> readBackDevices;
		readBackDevices = retval.getInfo<CL_CONTEXT_DEVICES>();
		messageManager()->sendDebug("Found "+QString::number(readBackDevices.size())+" device(s) in the new context.");

	} catch (cl::Error error)
	{
		messageManager()->sendError("Could not create a OpenCL context. Reason: "+QString(error.what()));
		check_error(error.err());
	}

	return retval;
}

cl::CommandQueue OpenCL::createCommandQueue(cl::Context context, cl::Device device)
{
	OpenCLPrinter::printDeviceInfo(device);
	cl::CommandQueue retval;
	try
	{
		retval = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, NULL);
		messageManager()->sendInfo("Created command queue using device "+qstring_cast(device.getInfo<CL_DEVICE_NAME>()));
	} catch(cl::Error error)
	{
		messageManager()->sendError("Could not create a OpenCL command queue. Reason: "+QString(error.what()));
		check_error(error.err());
	}

	return retval;
}

VECTOR_CLASS<cl::Device> OpenCL::getOnlyValidDevices(VECTOR_CLASS<cl::Device> devices)
{
	VECTOR_CLASS<cl::Device> valid;
	for(int i=0; i<devices.size(); ++i)
	{
		try
		{
			devices[i].getInfo<CL_DEVICE_NAME>();
			valid.push_back(devices[i]);

		} catch (cl::Error error)
		{
			messageManager()->sendWarning("Found invalid device. Device had no name.");
		}
	}
	return valid;
}


//------

//OpenCL::ocl* OpenCL::init(QString processor)
//{
//	cl_int err;
//
//	OpenCL::ocl* retval = new OpenCL::ocl;
//
//	// AMD way:
//	cl_platform_id platforms[10];
//	cl_uint numPlatforms = 10;
//	cl_uint foundPlatforms = 0;
//	check_error(clGetPlatformIDs(numPlatforms, platforms, &foundPlatforms));
//
//	if (foundPlatforms != 1)
//	{
//		messageManager()->sendWarning("The number of platforms found differs from 1. This might not be supported.");
//	}
//
//	cl_context_properties cps[3] =
//	{ CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[0], 0 };
//
//
//	if (processor == "CPU")
//	{
//		check_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &(retval->device), NULL));
//		//This callback function will be used by the OpenCL implementation to report information on errors that occur in this context.
//		retval->context = clCreateContextFromType(cps, CL_DEVICE_TYPE_CPU, errorCallback, NULL, &err);
//	}
//
//
//	else // GPU
//	{
//		cl_uint foundDevices = 0;
//		cl_device_id devices[10];
//		check_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 10, devices, &foundDevices));
//		// Pick the GPU with the most global memory
//		if (foundDevices == 0)
//		{
//			messageManager()->sendError("Did not find any GPU. Aborting.");
//			return NULL;
//		}
//		size_t largestMem = 0;
//		int chosenDev = 0;
//		size_t devMem = 0;
//		for (int i = 0; i < foundDevices; i++)
//		{
//			check_error(clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(size_t), &devMem, NULL));
//			messageManager()->sendInfo(
//							"Device " + QString(OpenCLUtilities::getDeviceString(devices[i]).c_str()) + " has "
//							+ QString::number(devMem) + " bytes (" + QString::number(devMem / 1024.0f / 1024.0f)
//							+ " MB) of memory.");
//			if (devMem > largestMem)
//			{
//				chosenDev = i;
//				largestMem = devMem;
//			}
//		}
//		//This callback function will be used by the OpenCL implementation to report information on errors that occur in this context.
//		retval->context = clCreateContext(cps, 1, &devices[chosenDev], errorCallback, NULL, &err);
//		retval->device = devices[chosenDev];
//	}
//
//
//	messageManager()->sendInfo("Using device " + QString(OpenCLUtilities::getDeviceString(retval->device).c_str()));
//	check_error(err);
//	retval->cmd_queue = clCreateCommandQueue(retval->context, retval->device, CL_QUEUE_PROFILING_ENABLE, &err);
//	check_error(err);
//
//	return retval;
//}

void OpenCL::release(OpenCL::ocl* ocl)
{
	//TODO release context->device;

//	clReleaseCommandQueue(ocl->cmd_queue);
//	clReleaseContext(ocl->context);
//	clUnloadCompiler();
	messageManager()->sendInfo("Releasing OpenCL context, device and command queue.");

	delete ocl;
}

//cl_kernel OpenCL::createKernel(cl_program program, cl_device_id device, const char * kernel_name)
cl::Kernel OpenCL::createKernel(cl::Program program, cl::Device device, const char * kernel_name)
{
	cl_int err = 0;
//	cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
//	check_error(err);
	cl::Kernel kernel(program, kernel_name, &err);
	check_error(err);

	return kernel;
}

//cl_mem OpenCL::createBuffer(cl_context context, cl_mem_flags flags, size_t size, void * host_data)
cl::Buffer OpenCL::createBuffer(cl::Context context, cl_mem_flags flags, size_t size, void * host_data, std::string bufferName)
{
	cl::Buffer dev_mem;
	try
	{
		if (host_data != NULL)
			flags |= CL_MEM_COPY_HOST_PTR;
		//cl_mem dev_mem = clCreateBuffer(context, flags, size, host_data, &err);
		messageManager()->sendDebug("Buffer size is "+QString::number(size)+" bytes ("+QString::number((double)size/(1024*1024))+" MB)");
		dev_mem = cl::Buffer(context, flags, size, host_data, NULL);
		dev_mem.setDestructorCallback(memoryDestructorCallback, static_cast<void*>(new std::string(bufferName)));
	//	if (err != CL_SUCCESS)
	//	{
	//		messageManager()->sendError("Could not create buffer of size " + QString::number(size) + " ("+ QString::number(size / 1024.0f / 1024.0f) + " MB, error:" + QString(err));
	//		exit(err);
	//	}
	} catch (cl::Error error)
	{
		messageManager()->sendError("Could not create a OpenCL buffer queue. Reason: "+QString(error.what()));
		check_error(error.err());
	}
	return dev_mem;
}

void OpenCL::checkBuildProgramLog(cl::Program program, cl::Device device, cl_int err)
{
	    cl::STRING_CLASS log;
	    program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &log);

		messageManager()->sendInfo("Build log: \n"+qstring_cast(log));
}
//================================================================================================================
//================================================================================================================


void OpenCLUtilities::generateOpenCLError(cl_int id, const char* file, int line)
{
	QString type;
	switch (id)
	{
	case CL_SUCCESS                        : type = "CL_SUCCESS"; break;
	case CL_DEVICE_NOT_FOUND               : type = "CL_DEVICE_NOT_FOUND"; break;
	case CL_DEVICE_NOT_AVAILABLE           : type = "CL_DEVICE_NOT_AVAILABLE"; break;
	case CL_COMPILER_NOT_AVAILABLE         : type = "CL_COMPILER_NOT_AVAILABLE"; break;
	case CL_MEM_OBJECT_ALLOCATION_FAILURE  : type = "CL_MEM_OBJECT_ALLOCATION_FAILURE"; break;
	case CL_OUT_OF_RESOURCES               : type = "CL_OUT_OF_RESOURCES"; break;
	case CL_OUT_OF_HOST_MEMORY             : type = "CL_OUT_OF_HOST_MEMORY"; break;
	case CL_PROFILING_INFO_NOT_AVAILABLE   : type = "CL_PROFILING_INFO_NOT_AVAILABLE"; break;
	case CL_MEM_COPY_OVERLAP               : type = "CL_MEM_COPY_OVERLAP"; break;
	case CL_IMAGE_FORMAT_MISMATCH          : type = "CL_IMAGE_FORMAT_MISMATCH"; break;
	case CL_IMAGE_FORMAT_NOT_SUPPORTED     : type = "CL_IMAGE_FORMAT_NOT_SUPPORTED"; break;
	case CL_BUILD_PROGRAM_FAILURE          : type = "CL_BUILD_PROGRAM_FAILURE"; break;
	case CL_MAP_FAILURE                    : type = "CL_MAP_FAILURE"; break;
//	case CL_MISALIGNED_SUB_BUFFER_OFFSET   : type = "CL_MISALIGNED_SUB_BUFFER_OFFSET"; break;
//	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST : type = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"; break;
	case CL_INVALID_VALUE                  : type = "CL_INVALID_VALUE"; break;
	case CL_INVALID_DEVICE_TYPE            : type = "CL_INVALID_DEVICE_TYPE"; break;
	case CL_INVALID_PLATFORM               : type = "CL_INVALID_PLATFORM"; break;
	case CL_INVALID_DEVICE                 : type = "CL_INVALID_DEVICE"; break;
	case CL_INVALID_CONTEXT                : type = "CL_INVALID_CONTEXT"; break;
	case CL_INVALID_QUEUE_PROPERTIES       : type = "CL_INVALID_QUEUE_PROPERTIES"; break;
	case CL_INVALID_COMMAND_QUEUE          : type = "CL_INVALID_COMMAND_QUEUE"; break;
	case CL_INVALID_HOST_PTR               : type = "CL_INVALID_HOST_PTR"; break;
	case CL_INVALID_MEM_OBJECT             : type = "CL_INVALID_MEM_OBJECT"; break;
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: type = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"; break;
	case CL_INVALID_IMAGE_SIZE             : type = "CL_INVALID_IMAGE_SIZE"; break;
	case CL_INVALID_SAMPLER                : type = "CL_INVALID_SAMPLER"; break;
	case CL_INVALID_BINARY                 : type = "CL_INVALID_BINARY"; break;
	case CL_INVALID_BUILD_OPTIONS          : type = "CL_INVALID_BUILD_OPTIONS"; break;
	case CL_INVALID_PROGRAM                : type = "CL_INVALID_PROGRAM"; break;
	case CL_INVALID_PROGRAM_EXECUTABLE     : type = "CL_INVALID_PROGRAM_EXECUTABLE"; break;
	case CL_INVALID_KERNEL_NAME            : type = "CL_INVALID_KERNEL_NAME"; break;
	case CL_INVALID_KERNEL_DEFINITION      : type = "CL_INVALID_KERNEL_DEFINITION"; break;
	case CL_INVALID_KERNEL                 : type = "CL_INVALID_KERNEL"; break;
	case CL_INVALID_ARG_INDEX              : type = "CL_INVALID_ARG_INDEX"; break;
	case CL_INVALID_ARG_VALUE              : type = "CL_INVALID_ARG_VALUE"; break;
	case CL_INVALID_ARG_SIZE               : type = "CL_INVALID_ARG_SIZE"; break;
	case CL_INVALID_KERNEL_ARGS            : type = "CL_INVALID_KERNEL_ARGS"; break;
	case CL_INVALID_WORK_DIMENSION         : type = "CL_INVALID_WORK_DIMENSION"; break;
	case CL_INVALID_WORK_GROUP_SIZE        : type = "CL_INVALID_WORK_GROUP_SIZE"; break;
	case CL_INVALID_WORK_ITEM_SIZE         : type = "CL_INVALID_WORK_ITEM_SIZE"; break;
	case CL_INVALID_GLOBAL_OFFSET          : type = "CL_INVALID_GLOBAL_OFFSET"; break;
	case CL_INVALID_EVENT_WAIT_LIST        : type = "CL_INVALID_EVENT_WAIT_LIST"; break;
	case CL_INVALID_EVENT                  : type = "CL_INVALID_EVENT"; break;
	case CL_INVALID_OPERATION              : type = "CL_INVALID_OPERATION"; break;
	case CL_INVALID_GL_OBJECT              : type = "CL_INVALID_GL_OBJECT"; break;
	case CL_INVALID_BUFFER_SIZE            : type = "CL_INVALID_BUFFER_SIZE"; break;
	case CL_INVALID_MIP_LEVEL              : type = "CL_INVALID_MIP_LEVEL"; break;
	case CL_INVALID_GLOBAL_WORK_SIZE       : type = "CL_INVALID_GLOBAL_WORK_SIZE"; break;
//	case CL_INVALID_PROPERTY               : type = "CL_INVALID_PROPERTY"; break;
	default                                : type = "unknown"; break;
	}

	QString err = QString("OpenCL ERROR[%1], file=%2[%3], msg=%4").arg(id).arg(file).arg(line).arg(type);
	messageManager()->sendError(err);

	//CustusX don«t
	//throw err.toStdString();
}

std::string OpenCLUtilities::getDeviceString(cl_device_id dev)
{
	std::string ret = "";
	char buf[256];
	size_t len;

	check_error(clGetDeviceInfo(dev, CL_DEVICE_VENDOR, sizeof(buf), buf, &len));
	ret += buf;
	ret += " ";
	check_error(clGetDeviceInfo(dev, CL_DEVICE_NAME, sizeof(buf), buf, &len));
	ret += buf;
	return ret;
}

char* OpenCLUtilities::file2string(const char* filename, size_t * final_length)
{
	FILE * file_stream = NULL;
	size_t source_length;

	// open the OpenCL source code file
	file_stream = fopen(filename, "rb");
	if (file_stream == NULL)
		return 0;

	fseek(file_stream, 0, SEEK_END);
	source_length = ftell(file_stream);
	fseek(file_stream, 0, SEEK_SET);

	char* source_str = (char *) malloc(source_length + 1); // TODO: Free
	memset(source_str, 0, source_length);

	if (fread(source_str, source_length, 1, file_stream) != 1)
	{
		fclose(file_stream);
		free(source_str);
		messageManager()->sendError("fread did not read " + QString(filename) + " correctly.");
		return 0;
	}

	fclose(file_stream);
	if (final_length != NULL)
		*final_length = source_length;

	source_str[source_length] = '\0';

	return source_str;
}

}//namespace cx


#endif //SSC_USE_OpenCL

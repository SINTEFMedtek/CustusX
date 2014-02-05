#ifdef SSC_USE_OpenCL

#include "cxOpenCLUtilities.h"

#include <iostream>
#include <QString>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"

#if CX_USE_OPENCL_UTILITY
	#include "OpenCLManager.hpp"
#endif

namespace cx
{

void CL_CALLBACK contextCallback(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	messageManager()->sendError("Context callback:\n " + QString(errinfo));
}

void CL_CALLBACK memoryDestructorCallback(cl_mem memobj, void* user_data)
{
	std::string* data_pointer = static_cast<std::string*>(user_data);
	std::string data = *data_pointer;
	messageManager()->sendInfo("Memory destructor callback: " + QString(data.c_str()));
}

OpenCL::ocl* OpenCL::init(cl_device_type type)
{

#if CX_USE_OPENCL_UTILITY
	oul::opencl();
#endif
	OpenCL::ocl* retval = NULL;

	try
	{
		cl::Platform platform = selectPlatform();
		cl::Device device = selectDevice(type, platform);
		cl_context_properties cps[] = { CL_CONTEXT_PLATFORM, (cl_context_properties) (platform)(), 0 };
		cl::Context context = createContext(device, cps);
		cl::CommandQueue commandQueue = createCommandQueue(context, device);

		retval = new OpenCL::ocl;
		retval->device = device;
		retval->context = context;
		retval->cmd_queue = commandQueue;

	}catch(cl::Error &error)
	{
		messageManager()->sendError("Could not initialize OpenCL. Reason: "+QString(error.what()));
		check_error(error.err());
	}

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

        std::string name, vendor, version;
		name = retval.getInfo<CL_PLATFORM_NAME>();
		vendor = retval.getInfo<CL_PLATFORM_VENDOR>();
		version = retval.getInfo<CL_PLATFORM_VERSION>();

		messageManager()->sendInfo("Selected platform "+qstring_cast(name)+" from vendor "+qstring_cast(vendor)+" using "+qstring_cast(version));

	}catch(cl::Error &error)
	{
		messageManager()->sendError("Could not select a OpenCL platform. Reason: "+QString(error.what()));
		check_error(error.err());
		throw error;
	}

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
		if(foundDevices == 0)
			throw cl::Error(1, std::string("No OpenCL devices of type "+typeString+" found on this platform.").c_str());

		retval = chooseDeviceWithMostGlobalMemory(devices);

		cl::STRING_CLASS name, vendor, version;
		name = retval.getInfo<CL_DEVICE_NAME>();
		vendor = retval.getInfo<CL_DEVICE_VENDOR>();
		version = retval.getInfo<CL_DEVICE_VERSION>();
		messageManager()->sendInfo("Selected device "+qstring_cast(name)+" from vendor "+qstring_cast(vendor)+" using "+qstring_cast(version));

	}catch(cl::Error &error)
	{
		messageManager()->sendError("Could not select a OpenCL device. Reason: "+QString(error.what()));
		check_error(error.err());
		throw error;
	}

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
		} catch (cl::Error &error)
		{
			messageManager()->sendWarning("Could not ask device about CL_DEVICE_GLOBAL_MEM_SIZE. Reason: "+QString(error.what()));
			check_error(error.err());
			throw error;
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
		retval = cl::Context(devices, cps, contextCallback, NULL, NULL);
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

	} catch (cl::Error &error)
	{
		messageManager()->sendError("Could not create a OpenCL context. Reason: "+QString(error.what()));
		check_error(error.err());
		throw error;
	}

	return retval;
}

cl::CommandQueue OpenCL::createCommandQueue(cl::Context context, cl::Device device)
{
	cl::CommandQueue retval;
	try
	{
		retval = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, NULL);
		messageManager()->sendInfo("Created command queue using device "+qstring_cast(device.getInfo<CL_DEVICE_NAME>()));
	} catch(cl::Error &error)
	{
		messageManager()->sendError("Could not create a OpenCL command queue. Reason: "+QString(error.what()));
		check_error(error.err());
		throw error;
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

		} catch (cl::Error &error)
		{
			messageManager()->sendWarning("Found invalid device. Device had no name.");
			check_error(error.err());
			throw error;
		}
	}
	return valid;
}

void OpenCL::release(OpenCL::ocl* ocl)
{
	messageManager()->sendInfo("Releasing OpenCL context, device and command queue.");

	cl::UnloadCompiler(); //TODO is this needed???
	if(ocl != NULL)
	{
		delete ocl;
		ocl = NULL;
	}
}

cl::Program OpenCL::createProgram(cl::Context context, const char* source, size_t sourceLength)
{
	cl::Program retval;
	try
	{
		cl::Program::Sources sources;
		sources.push_back(std::pair<const char*, ::size_t>(source, sourceLength));
		retval = cl::Program(context, sources);
		messageManager()->sendInfo("Created program.");
	}
	catch (cl::Error &error)
	{
		messageManager()->sendError("Could not create a OpenCL program queue. Reason: "+QString(error.what()));
		check_error(error.err());
		throw error;
	}
	return retval;
}

void OpenCL::build(cl::Program program, QString buildOptions)
{
	VECTOR_CLASS<cl::Device> devices;
	try
	{
		cl::Context context(program.getInfo<CL_PROGRAM_CONTEXT>());
		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		if(devices.size() == 0)
			messageManager()->sendError("Device is NULL.");
		program.build(devices, buildOptions.toStdString().c_str(), NULL, NULL);
	}
	catch(cl::Error &error)
	{
		messageManager()->sendError("Could not build program. Reason: "+QString(error.what()));
		for(int i=0; i<devices.size(); i++)
		{
			checkBuildProgramLog(program, devices[i], error.err());
		}
		//check_error(error.err());
	}
}

cl::Kernel OpenCL::createKernel(cl::Program program, const char * kernel_name)
{
	cl::Kernel kernel;
	if(kernel_name == NULL)
		messageManager()->sendError("kernel_name is NULL...");
	try
	{
		kernel = cl::Kernel(program, kernel_name, NULL);
		messageManager()->sendInfo("Created kernel with name "+QString(kernel_name));
	}
	catch(cl::Error &error)
	{
		messageManager()->sendError("Could not create kernel. Reason:"+QString(error.what()));
		check_error(error.err());
	}

	return kernel;
}

cl::Buffer OpenCL::createBuffer(cl::Context context, cl_mem_flags flags, size_t size, void * host_data, std::string bufferName)
{
	cl::Buffer dev_mem;
	try
	{
		if (host_data != NULL)
			flags |= CL_MEM_COPY_HOST_PTR;
		dev_mem = cl::Buffer(context, flags, size, host_data, NULL);
		dev_mem.setDestructorCallback(memoryDestructorCallback, static_cast<void*>(new std::string(bufferName)));
	} catch (cl::Error &error)
	{
		messageManager()->sendError("Could not create a OpenCL buffer queue. Reason: "+QString(error.what()));
		check_error(error.err());
		throw error;
	}
	return dev_mem;
}

void OpenCL::checkBuildProgramLog(cl::Program program, cl::Device device, cl_int err)
{
	    cl::STRING_CLASS log;
	    program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &log);

		messageManager()->sendInfo("Build log: \n"+qstring_cast(log));
}

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
	{
		messageManager()->sendError("Could not read kernel file: "+QString(filename));
		return 0;
	}

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

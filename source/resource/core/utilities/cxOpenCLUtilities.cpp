#ifdef SSC_USE_OpenCL

#include "cxOpenCLUtilities.h"

#include <iostream>
#include <QString>
#include "sscMessageManager.h"

namespace cx
{

void CL_CALLBACK errorCallback(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	messageManager()->sendError("Error callback: " + QString(errinfo));
}

OpenCL::

OpenCL::context* OpenCL::init(QString processor)
{
	cl_int err;

	OpenCL::context* retval = new OpenCL::context;

	// AMD way:
	cl_platform_id platforms[10];
	cl_uint numPlatforms = 10;
	cl_uint foundPlatforms = 0;
	check_error(clGetPlatformIDs(numPlatforms, platforms, &foundPlatforms));

	if (foundPlatforms != 1)
	{
		messageManager()->sendWarning("The number of platforms found differs from 1. This might not be supported.");
	}

	cl_context_properties cps[3] =
	{ CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[0], 0 };


	if (processor == "CPU")
	{
		check_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &(retval->device), NULL));
		//This callback function will be used by the OpenCL implementation to report information on errors that occur in this context.
		retval->context = clCreateContextFromType(cps, CL_DEVICE_TYPE_CPU, errorCallback, NULL, &err);
	}


	else // GPU
	{
		cl_uint foundDevices = 0;
		cl_device_id devices[10];
		check_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 10, devices, &foundDevices));
		// Pick the GPU with the most global memory
		if (foundDevices == 0)
		{
			messageManager()->sendError("Did not find any GPU. Aborting.");
			return NULL;
		}
		size_t largestMem = 0;
		int chosenDev = 0;
		size_t devMem = 0;
		for (int i = 0; i < foundDevices; i++)
		{
			check_error(clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(size_t), &devMem, NULL));
			messageManager()->sendInfo(
							"Device " + QString(OpenCLUtilities::getDeviceString(devices[i]).c_str()) + " has "
							+ QString::number(devMem) + " bytes (" + QString::number(devMem / 1024.0f / 1024.0f)
							+ " MB) of memory.");
			if (devMem > largestMem)
			{
				chosenDev = i;
				largestMem = devMem;
			}
		}
		//This callback function will be used by the OpenCL implementation to report information on errors that occur in this context.
		retval->context = clCreateContext(cps, 1, &devices[chosenDev], errorCallback, NULL, &err);
		retval->device = devices[chosenDev];
	}


	messageManager()->sendInfo("Using device " + QString(OpenCLUtilities::getDeviceString(retval->device).c_str()));
	check_error(err);
	retval->cmd_queue = clCreateCommandQueue(retval->context, retval->device, CL_QUEUE_PROFILING_ENABLE, &err);
	check_error(err);

	return retval;
}

void OpenCL::release(OpenCL::context* context)
{
	//TODO release context->device;

	clReleaseCommandQueue(context->cmd_queue);
	clReleaseContext(context->context);
	clUnloadCompiler();
}

cl_kernel OpenCL::createKernel(cl_program program, cl_device_id device, const char * kernel_name)
{
	cl_int err;
	cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
	check_error(err);

	return kernel;
}

cl_mem OpenCL::createBuffer(cl_context context, cl_mem_flags flags, size_t size, void * host_data)
{
	if (host_data != NULL)
		flags |= CL_MEM_COPY_HOST_PTR;
	cl_int err;
	cl_mem dev_mem = clCreateBuffer(context, flags, size, host_data, &err);
	if (err != CL_SUCCESS)
	{
		messageManager()->sendError(
				"Could not create buffer of size " + QString::number(size) + " ("
						+ QString::number(size / 1024.0f / 1024.0f) + " MB, error:" + QString(err));
		exit(err);
	}
	return dev_mem;
}

//void OpenCL::checkBuildProgramLog(cl_program program, cl_device_id device, cl_int err)
//{
//	if(err != CL_SUCCESS)
//	{
//		messageManager()->sendError("Building program failed.");
//
//		// Determine the size of the log
//		size_t log_size;
//		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
//
//		// Allocate memory for the log
//		char *log = (char *) malloc(log_size);
//
//		// Get the log
//		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
//
//		// Print the log
//		messageManager()->sendInfo("Build log: "+QString(log));
//	}
//}

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

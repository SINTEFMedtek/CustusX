#ifdef SSC_USE_OpenCL

#include "cxOpenCLUtilities.h"

#include <iostream>
#include <QString>
#include "sscMessageManager.h"

namespace cx
{

/**
 *  See clCreateContextFromType doc for more
 */
void CL_CALLBACK clCreateContextFromType_error_callback(const char *errinfo, const void  *private_info, size_t  cb, void  *user_data)
{
	std::cout << "ERROR: From clCreateContextFromType() callback: "<< errinfo << std::endl;
}

OpenCL::ocl_context* OpenCL::ocl_init(QString processor)
{
	cl_int err;

	OpenCL::ocl_context* retval = new OpenCL::ocl_context;

	// AMD way:
	cl_platform_id platforms[10];
	cl_uint numPlatforms = 10;
	cl_uint foundPlatforms = 0;
	ocl_check_error(clGetPlatformIDs(numPlatforms, platforms, &foundPlatforms));
	if(foundPlatforms != 1)
	{
		std::cerr << "WARNING: " << foundPlatforms << " OpenCL platforms found, which differs from 1!\n";
	}
	cl_context_properties cps[3] =
	{ CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[0], 0 };
	if (processor == "CPU")
	{
		ocl_check_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &(retval->device), NULL));
		retval->context = clCreateContextFromType(cps, CL_DEVICE_TYPE_CPU, clCreateContextFromType_error_callback, NULL, &err);
	}
	else // GPU
	{
		cl_uint foundDevices = 0;
		cl_device_id devices[10];
		ocl_check_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 10, devices, &foundDevices));
		// Pick the GPU with the most global memory
		if(foundDevices == 0)
		{
			std::cerr << "Did not find any GPU-! Aborting\n";
			return NULL;
		}
		size_t largestMem = 0;
		int chosenDev = 0;
		size_t devMem = 0;
		for(int i = 0; i < foundDevices; i++)
		{
			ocl_check_error(clGetDeviceInfo(devices[i],
			                                CL_DEVICE_GLOBAL_MEM_SIZE,
			                                sizeof(size_t),
			                                &devMem,
			                                NULL));
			std::cerr << "Device " << OpenCLUtilities::ocl_get_device_string(devices[i]) << " has " << devMem << " bytes of memory\n";
			if(devMem > largestMem)
			{
				chosenDev = i;
				largestMem = devMem;
			}
		}
		retval->context = clCreateContext(cps, 1, &devices[chosenDev],
		                                  clCreateContextFromType_error_callback,
		                                  NULL, &err);
		retval->device = devices[chosenDev];
	}
	std::cerr << "Using device " << OpenCLUtilities::ocl_get_device_string(retval->device) << std::endl;
	ocl_check_error(err);
	retval->cmd_queue = clCreateCommandQueue(retval->context, retval->device, CL_QUEUE_PROFILING_ENABLE, &err);
	ocl_check_error(err);

	return retval;
}

void OpenCL::ocl_release(OpenCL::ocl_context* context)
{
	//TODO release context->device;

	clReleaseCommandQueue(context->cmd_queue);
	clReleaseContext(context->context);
	clUnloadCompiler();
}

cl_kernel OpenCL::ocl_kernel_build(cl_program program, cl_device_id device, const char * kernel_name) {
	cl_int err;
	cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
	//	if (err != CL_SUCCESS) {
		size_t len = 0;
		char buffer[2048];
		//printf("ERROR: Failed to build kernel %s on device %p. Error code: %d\n", kernel_name, device, err);

		clGetProgramBuildInfo(
			program,              // the program object being queried
			device,            // the device for which the OpenCL code was built
			CL_PROGRAM_BUILD_LOG, // specifies that we want the build log
			sizeof(buffer),       // the size of the buffer
			buffer,               // on return, holds the build log
			&len);                // on return, the actual size in bytes of the data returned

		if(len != 0)
		{
			buffer[len] = 0;
			printf("%s\n", buffer);
		}

		// for (int i = 0; i < 2048; i++)
		// 	printf("%c", buffer[i]);
		// printf("\n");
		//		exit(1);
		//	}
	return kernel;
}

cl_mem OpenCL::ocl_create_buffer(cl_context context, cl_mem_flags flags, size_t size, void * host_data) {
	if (host_data != NULL) flags |= CL_MEM_COPY_HOST_PTR;
	cl_int err;
	cl_mem dev_mem = clCreateBuffer(context, flags, size, host_data, &err);
	if (err != CL_SUCCESS) {
		printf("ERROR clCreateBuffer of size %lu bytes (%f MB): %d\n", size, size/1024.0f/1024.0f, err);
		exit(err);
	}
	//printf("Successfull clCreateBuffer of size %lu bytes (%f MB)\n", size, size/1024.0f/1024.0f);
	return dev_mem;
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
	std::cerr << err.toStdString() << std::endl;
	throw err.toStdString();
}

std::string OpenCLUtilities::ocl_get_device_string(cl_device_id dev)
{
	std::string ret = "";
	char buf[256];
	size_t len;

	ocl_check_error(clGetDeviceInfo(dev,
	                                CL_DEVICE_VENDOR,
	                                sizeof(buf),
	                                buf,
	                                &len));
	ret += buf;
	ret += " ";
	ocl_check_error(clGetDeviceInfo(dev,
	                                CL_DEVICE_NAME,
	                                sizeof(buf),
	                                buf,
	                                &len));
	ret += buf;
 	return ret;
}

char* OpenCLUtilities::file2string(const char* filename, size_t * final_length) {
	FILE * file_stream = NULL;
	size_t source_length;

	// open the OpenCL source code file
	file_stream = fopen(filename, "rb");
	if(file_stream == NULL) return 0;

	fseek(file_stream, 0, SEEK_END);
	source_length = ftell(file_stream);
	fseek(file_stream, 0, SEEK_SET);

	char* source_str = (char *)malloc(source_length + 1); // TODO: Free
	memset(source_str, 0, source_length);

	if (fread(source_str, source_length, 1, file_stream) != 1) {
		fclose(file_stream);
		free(source_str);
		printf("ERROR: fread did not read file %s correctly\n", filename);
		return 0;
	}

	fclose(file_stream);
	if(final_length != NULL)
		*final_length = source_length;

	source_str[source_length] = '\0';

	//printf("file2string returns size %d string: \n%s\n", source_length, source_str);

	return source_str;
}

}//namespace cx


#endif //SSC_USE_OpenCL

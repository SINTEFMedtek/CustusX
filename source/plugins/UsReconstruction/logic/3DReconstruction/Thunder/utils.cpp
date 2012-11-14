// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


#include "sscTypeConversions.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__
#include <string.h>
#include <math.h>
#include "utils.h"
#include <QString>
#include <iostream>

void generateOpenCLError(cl_int id, const char* file, int line)
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

float3 cross(float3 v, float3 w) {
	float3 c = {
		v.y*w.z - v.z*w.y,
		v.z*w.x - v.x*w.z,
		v.x*w.y - v.y*w.x
	};
	return c;
}

float3 sub(float3 v, float3 w) {
	float3 c = {
		v.x - w.x,
		v.y - w.y,
		v.z - w.z
	};
	return c;
}

float3 normalize(float3 v) {
	float length = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	float3 c = {
		v.x/length,
		v.y/length,
		v.z/length
	};
	return c;
}

float3 add(float3 v, float3 w) {
	float3 c = {
		v.x + w.x,
		v.y + w.y,
		v.z + w.z
	};
	return c;
}

float3 scale(float a, float3 v) {
	float3 c = {
		a * v.x,
		a * v.y,
		a * v.z
	};
	return c;
}

float dot(float3 v, float3 w) {
	return v.x*w.x + v.y*w.y + v.z*w.z;
}

void random_init(float * data, int length) {
	for (int i = 0; i < length; i++)
		data[i] = rand()/(float)RAND_MAX;
}

void inc_init(float * data, int length) {
	for (int i = 0; i < length; i++)
		data[i] = i;
}

char* file2string(const char* filename, size_t * final_length) {
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

cl_kernel ocl_kernel_build(cl_program program, cl_device_id device, const char * kernel_name) {
	cl_int err;
	cl_kernel kernel = clCreateKernel(program, kernel_name, &err);
	if (err != CL_SUCCESS) {
		size_t len;
		char buffer[2048];
		printf("ERROR: Failed to build kernel %s on device %p. Error code: %d\n", kernel_name, device, err);

		clGetProgramBuildInfo(
			program,              // the program object being queried
			device,            // the device for which the OpenCL code was built
			CL_PROGRAM_BUILD_LOG, // specifies that we want the build log
			sizeof(buffer),       // the size of the buffer
			buffer,               // on return, holds the build log
			&len);                // on return, the actual size in bytes of the data returned

		printf("%s\n", buffer);
		for (int i = 0; i < 2048; i++)
			printf("%c", buffer[i]);
		printf("\n");
		exit(1);
	}
	return kernel;
}

cl_mem ocl_create_buffer(cl_context context, cl_mem_flags flags, size_t size, void * host_data) {
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

//void ocl_check_error(int err, const char * info) {
//	if (err != CL_SUCCESS) {
//		printf("ERROR %s: %d\n", info, err);
//		exit(err);
//	}
//}

/** use this to check if one of the devices GPU or CPU,
 *  are available on the system.
 *
 * corresponding to cl values
 *     CL_DEVICE_TYPE_CPU
 *     CL_DEVICE_TYPE_GPU
 */
bool ocl_has_device_type(QString processor)
{
	uint device_type = 0;
	if (processor=="CPU")
		device_type = CL_DEVICE_TYPE_CPU;
	if (processor=="GPU")
		device_type = CL_DEVICE_TYPE_GPU;

	cl_uint platforms_n;
	cl_uint devices_n;
	size_t temp_size;
	cl_platform_id platform;
	cl_device_id * devices = (cl_device_id *) malloc(sizeof(cl_device_id) * 256);

	clGetPlatformIDs(1, &platform, &platforms_n);
	if (platforms_n<1)
	{
		free(devices);
		return false;
	}

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 256, devices, &devices_n);
	for (unsigned int j = 0; j < devices_n; j++)
	{
		cl_device_type type;
		clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(type), &type, &temp_size);
		if (type == device_type)
		{
			free(devices);
			return true;
		}
	}

	free(devices);
	return false;
}

/**print info on opencl hardware to stdout.
 *
 */
void ocl_print_info()
{
	cl_uint platforms_n;
	cl_uint devices_n;
	size_t temp_size;
	cl_platform_id * platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id)*256);
	cl_device_id * devices = (cl_device_id *) malloc(sizeof(cl_device_id)*256);
	char * str = (char *) malloc(sizeof(char)*2048);

	clGetPlatformIDs(256, platforms, &platforms_n);
	for (unsigned int i = 0; i < platforms_n; i++) {
		std::cout << QString("platform %1 of %2:").arg(i+1).arg(platforms_n) << std::endl;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 2048, str, &temp_size);
		std::cout << QString("\t CL_PLATFORM_VERSION: %1").arg(str) << std::endl;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 2048, str, &temp_size);
		std::cout << QString("\t CL_PLATFORM_NAME: %1").arg(str) << std::endl;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 2048, str, &temp_size);
		std::cout << QString("\t CL_PLATFORM_VENDOR: %1").arg(str) << std::endl;

		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 256, devices, &devices_n);
		for (unsigned int j = 0; j < devices_n; j++) {
			std::cout << QString("\t device %1 of %2:").arg(j+1).arg(devices_n) << std::endl;
			cl_device_type type;
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(type), &type, &temp_size);
			if (type == CL_DEVICE_TYPE_CPU)
				std::cout << QString("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_CPU") << std::endl;
			else if (type == CL_DEVICE_TYPE_GPU)
				std::cout << QString("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_GPU") << std::endl;
			else if (type == CL_DEVICE_TYPE_ACCELERATOR)
				std::cout << QString("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_ACCELERATOR") << std::endl;
			else if (type == CL_DEVICE_TYPE_DEFAULT)
				std::cout << QString("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_DEFAULT") << std::endl;
			else 
				std::cout << QString("\t\t CL_DEVICE_TYPE: (combination)") << std::endl;

			cl_uint temp_uint;
			cl_ulong temp_ulong;
			size_t temp_size_t;
			cl_bool temp_bool;
			size_t * size_t_array = (size_t *) malloc(sizeof(size_t)*3);
			std::cout << QString("\t\t device id: %1").arg((long int)devices[j]) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(temp_uint), &temp_uint, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_MAX_CLOCK_FREQUENCY: %1 MHz").arg(temp_uint) << std::endl;

			std::cout << std::endl;
			std::cout << QString("\tMemory info:") << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(temp_ulong), &temp_ulong, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_GLOBAL_MEM_SIZE: %1 Mb").arg(temp_ulong/1024/1024) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(temp_ulong), &temp_ulong, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_MAX_MEM_ALLOC_SIZE: %1 Mb").arg(temp_ulong/1024/1024) << std::endl;
//			clGetDeviceInfo(devices[j], CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &temp_bool, &temp_size);
//			printf("\t\t CL_DEVICE_HOST_UNIFIED_MEMORY: %u \n", temp_bool);

			std::cout << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(temp_uint), &temp_uint, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_MAX_COMPUTE_UNITS: %1").arg(temp_uint) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(temp_uint), &temp_uint, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %1").arg(temp_uint) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*3, size_t_array, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_MAX_WORK_ITEM_SIZES: %1 %2 %3").arg(size_t_array[0]).arg(size_t_array[1]).arg(size_t_array[2]) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(temp_size_t), &temp_size_t, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_MAX_WORK_GROUP_SIZE: %1").arg(temp_size_t) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(temp_uint), &temp_uint, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_ADDRESS_BITS: %1").arg(temp_uint) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(temp_size_t), &temp_size_t, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_MAX_PARAMETER_SIZE: %1 b").arg(temp_size_t) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(temp_ulong), &temp_ulong, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_LOCAL_MEM_SIZE: %1 b").arg(temp_ulong) << std::endl;

			std::cout << std::endl;
			std::cout << QString("\tDevice info:") << std::endl;

			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 2048, str, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_NAME: %1").arg(str) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, 2048, str, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_VENDOR: %1").arg(str) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 2048, str, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_VERSION: %1").arg(str) << std::endl;
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 2048, str, &temp_size);
			std::cout << QString("\t\t CL_DRIVER_VERSION: %1").arg(str) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, sizeof(cl_bool), &temp_bool, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_AVAILABLE: %1").arg(temp_bool) << std::endl;
			clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 2048, str, &temp_size);
			std::cout << QString("\t\t CL_DEVICE_EXTENSIONS: %1").arg(str) << std::endl;
		}
	}
	std::cout << std::endl;
}



/*void ocl_set_args(cl_kernel kernel, int n, ...) {
	va_list args;
	va_start(args, n);
	for (int i = 0; i < n; i++) {
		void * arg = va_arg(args, void *);
		clSetKernelArg(kernel, i, sizeof(arg), &arg);
	}
	va_end(args);
}*/

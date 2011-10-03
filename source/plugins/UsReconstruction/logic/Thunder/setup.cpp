/*
 *  setup.c
 *
 *  Created by Ole Vegard Solberg on 5/6/10.
 *
 */

#include "setup.h"

//#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
//#include <math.h>
#include <QString>
#include <iostream>
#include "holger_time.h"
#include "utils.h"

cl_program ocl_create_program(cl_context context, cl_device_id device, const char* program_src, QString kernel_path)
{
	cl_program retval;
	cl_int err;
	retval = clCreateProgramWithSource(context, 1, (const char **) &program_src, 0, &err);

	ocl_check_error(err);
	err = clBuildProgram(retval, 0, NULL, 0, 0, 0);
	if (err != CL_SUCCESS)
	{
		size_t len;
		char buffer[512 * 512];
		memset(buffer, 0, 512 * 512);
		printf("OpenCL ERROR: Failed to build program on device %p. Error code: %d\n", device, err);
		printf("Build log for program %s:\n", kernel_path.toStdString().c_str());

		clGetProgramBuildInfo(retval, // the program object being queried
			device, // the device for which the OpenCL code was built
			CL_PROGRAM_BUILD_LOG, // specifies that we want the build log
			sizeof(char) * 512 * 512, // the size of the buffer
			buffer, // on return, holds the build log
			&len); // on return, the actual size in bytes of the data returned

		printf("%lu %s\n", len, buffer);
		for (uint i = 0; i < len; i++)
			printf("%c", buffer[i]);
		printf("\n");
		exit(1);
	}
	return retval;
}

ocl_context* ocl_init(QString processor)
{
	cl_int err;

	ocl_context* retval = new ocl_context;

	/* // Old way:
	 context = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, 0, 0, &err);
	 ocl_check_error(err, "clCreateContextFromType");
	 size_t context_descriptor_size;
	 clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, 0, &context_descriptor_size);
	 cl_device_id * devices = (cl_device_id *) malloc(context_descriptor_size);
	 clGetContextInfo(context, CL_CONTEXT_DEVICES, context_descriptor_size, devices, 0);
	 device = devices[0];
	 */

	// New way:
	//clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	//context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

	// AMD way:
	cl_platform_id platform = NULL;
	cl_uint numPlatforms = 1;
	cl_uint foundPlatforms = 0;
	ocl_check_error(clGetPlatformIDs(numPlatforms, &platform, &foundPlatforms));
	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0 };
	if (processor == "CPU")
	{
		ocl_check_error(clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &(retval->device), NULL));
		retval->context = clCreateContextFromType(cps, CL_DEVICE_TYPE_CPU, NULL, NULL, &err);
	}
	else // GPU
	{
		ocl_check_error(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &(retval->device), NULL));
		retval->context = clCreateContextFromType(cps, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
	}

	ocl_check_error(err);
	retval->cmd_queue = clCreateCommandQueue(retval->context, retval->device, CL_QUEUE_PROFILING_ENABLE, &err);
	ocl_check_error(err);

	return retval;
}

void ocl_release(ocl_context* context)
{
//	printf("Thunder: ocl_release\n");

	//TODO release context->device;

	clReleaseCommandQueue(context->cmd_queue);
	clReleaseContext(context->context);
	clUnloadCompiler();
}

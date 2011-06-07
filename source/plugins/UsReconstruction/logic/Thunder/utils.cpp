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
	printf("Successfull clCreateBuffer of size %lu bytes (%f MB)\n", size, size/1024.0f/1024.0f);
	return dev_mem;
}

void ocl_check_error(int err, const char * info) {
	if (err != CL_SUCCESS) {
		printf("ERROR %s: %d\n", info, err);
		exit(err);
	}
}

void ocl_print_info() {
	cl_uint platforms_n;
	cl_uint devices_n;
	size_t temp_size;
	cl_platform_id * platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id)*256);
	cl_device_id * devices = (cl_device_id *) malloc(sizeof(cl_device_id)*256);
	char * str = (char *) malloc(sizeof(char)*2048);

	clGetPlatformIDs(256, platforms, &platforms_n);
	for (unsigned int i = 0; i < platforms_n; i++) {
		printf("platform %d of %d:\n", i+1, platforms_n);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 2048, str, &temp_size);
		printf("\t CL_PLATFORM_VERSION: %s\n", str);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 2048, str, &temp_size);
		printf("\t CL_PLATFORM_NAME: %s\n", str);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 2048, str, &temp_size);
		printf("\t CL_PLATFORM_VENDOR: %s\n", str);

		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 256, devices, &devices_n);
		for (unsigned int j = 0; j < devices_n; j++) {
			printf("\t device %d of %d:\n", j+1, devices_n);
			cl_device_type type;
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(type), &type, &temp_size);
			if (type == CL_DEVICE_TYPE_CPU)
				printf("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_CPU\n");
			else if (type == CL_DEVICE_TYPE_GPU)
				printf("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_GPU\n");
			else if (type == CL_DEVICE_TYPE_ACCELERATOR)
				printf("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_ACCELERATOR\n");
			else if (type == CL_DEVICE_TYPE_DEFAULT)
				printf("\t\t CL_DEVICE_TYPE: CL_DEVICE_TYPE_DEFAULT\n");
			else 
				printf("\t\t CL_DEVICE_TYPE: (combination)\n");

			cl_uint temp_uint;
			cl_ulong temp_ulong;
			size_t temp_size_t;
			size_t * size_t_array = (size_t *) malloc(sizeof(size_t)*3);
			printf("\t\t device id: %p\n", devices[j]);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(temp_uint), &temp_uint, &temp_size);
			printf("\t\t CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", temp_uint);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(temp_uint), &temp_uint, &temp_size);
			printf("\t\t CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", temp_uint);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*3, size_t_array, &temp_size);
			printf("\t\t CL_DEVICE_MAX_WORK_ITEM_SIZES: %lu %lu %lu\n", size_t_array[0], size_t_array[1], size_t_array[2]);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(temp_size_t), &temp_size_t, &temp_size);
			printf("\t\t CL_DEVICE_MAX_WORK_GROUP_SIZE: %lu\n", temp_size_t);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(temp_uint), &temp_uint, &temp_size);
			printf("\t\t CL_DEVICE_MAX_CLOCK_FREQUENCY: %d\n", temp_uint);
			clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(temp_uint), &temp_uint, &temp_size);
			printf("\t\t CL_DEVICE_ADDRESS_BITS: %d\n", temp_uint);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(temp_ulong), &temp_ulong, &temp_size);
			printf("\t\t CL_DEVICE_MAX_MEM_ALLOC_SIZE: %llu\n", temp_ulong);
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(temp_size_t), &temp_size_t, &temp_size);
			printf("\t\t CL_DEVICE_MAX_PARAMETER_SIZE: %lu\n", temp_size_t);
			clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(temp_ulong), &temp_ulong, &temp_size);
			printf("\t\t CL_DEVICE_GLOBAL_MEM_SIZE: %llu\n", temp_ulong);
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 2048, str, &temp_size);
			printf("\t\t CL_DEVICE_NAME: %s\n", str);
			clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, 2048, str, &temp_size);
			printf("\t\t CL_DEVICE_VENDOR: %s\n", str);
			clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 2048, str, &temp_size);
			printf("\t\t CL_DEVICE_EXTENSIONS: %s\n", str);
		}
	}
	printf("\n");
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
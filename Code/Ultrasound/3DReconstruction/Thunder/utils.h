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

#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__
#include <QString>

/**
 * \addtogroup sscThunder
 * \{
 */

void random_init(float * data, int length);
void inc_init(float * data, int length);
char * file2string(const char* filename, size_t * final_length = NULL);
cl_kernel ocl_kernel_build(cl_program program, cl_device_id device, const char * kernel_name);
void ocl_program_build(cl_program program, cl_uint num_devices = 0, const cl_device_id * device_list = NULL);
cl_mem ocl_create_buffer(cl_context context, cl_mem_flags flags, size_t size, void * host_data);
void generateOpenCLError(cl_int id, const char* file, int line);


#define ocl_check_error(id) 													\
{																				\
	if (id != CL_SUCCESS) 														\
	{																			\
		generateOpenCLError(id, __FILE__, __LINE__);				            \
	}																			\
}
//printf("OpenCL ERROR: %d in %s [%d]\n", id, __FILE__, __LINE__);        \


void ocl_print_info();
//void ocl_set_args(cl_kernel kernel, int n, ...);
bool ocl_has_device_type(QString processor);

typedef struct
{
	float x;
	float y;
	float z;
} float3;

typedef struct
{
	float a;
	float b;
	float c;
	float d;
} plane_eq;

float3 cross(float3 v, float3 w);
float3 sub(float3 v, float3 w);
float3 normalize(float3 v);
float3 add(float3 v, float3 w);
float3 scale(float a, float3 v);
float dot(float3 v, float3 w);
#define distance(v, plane) (plane.a*v.x + plane.b*v.y + plane.c*v.z + plane.d)/sqrt(plane.a*plane.a + plane.b*plane.b + plane.c*plane.c)

/**
 * \}
 */

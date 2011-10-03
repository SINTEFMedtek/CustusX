/*
 *  setup.h
 *
 *  Created by Ole Vegard Solberg on 5/6/10.
 *
 */

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__
class QString;

struct ocl_context
{
	cl_context context;
	cl_device_id device;
	cl_command_queue cmd_queue;
};

cl_program ocl_create_program(cl_context context, cl_device_id device, const char* program_src, QString kernel_path);

ocl_context* ocl_init(QString processor);

void ocl_release(ocl_context* context);

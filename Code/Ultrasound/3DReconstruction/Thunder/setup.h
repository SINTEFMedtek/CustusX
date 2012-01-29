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



#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__
class QString;

/**
 * \addtogroup sscThunder
 * \{
 */

struct ocl_context
{
	cl_context context;
	cl_device_id device;
	cl_command_queue cmd_queue;
};

cl_program ocl_create_program(cl_context context, cl_device_id device, const char* program_src, QString kernel_path);

ocl_context* ocl_init(QString processor);

void ocl_release(ocl_context* context);

/**
 * \}
 */

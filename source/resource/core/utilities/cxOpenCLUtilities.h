#ifndef CXOPENCLUTILITIES_H_
#define CXOPENCLUTILITIES_H_

#define __CL_ENABLE_EXCEPTIONS //telling the opencl c++ wrapper to throw exceptions

#include "OpenCLManager.hpp"
#include <string>

class QString;

namespace cx
{
static void CL_CALLBACK memoryDestructorCallback(cl_mem memobj, void* user_data);

/**
 * \brief Functionality for working with OpenCL
 *
 * NOTE: Based on Thunder
 * NOTE: Written using OpenCL 1.1
 *
 * \date Dec 10, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class OpenCL
{
public:
	struct ocl
	{
		cl::Context context;
		cl::Device device;
		cl::CommandQueue cmd_queue;
	};

	static ocl* init(cl_device_type type);
	static void release(ocl* ocl);

	static cl::Program createProgram(cl::Context context, const char* source, size_t sourceLength);
	static cl::Kernel createKernel(cl::Program program, const char * kernel_name);
	static cl::Buffer createBuffer(cl::Context context, cl_mem_flags flags, size_t size, void * host_data, std::string bufferName);

	static void build(cl::Program program, QString buildOptions); //TODO return error value or throw???
	static void checkBuildProgramLog(cl::Program program, cl::Device device, cl_int err);
	static void executeKernel(cl::CommandQueue queue, cl::Kernel kernel, size_t global_work_size, size_t local_work_size);
	static void readResultingVolume(cl::CommandQueue queue, cl::Buffer outputBuffer, size_t outputVolumeSize, void *outputData);

private:
	static cl::Platform selectPlatform();
	static cl::Device selectDevice(cl_device_type type, cl::Platform platform);
	static cl::Device chooseDeviceWithMostGlobalMemory(VECTOR_CLASS<cl::Device> devices);
	static cl::Context createContext(cl::Device device, cl_context_properties* cps);
	static cl::Context createContext(const VECTOR_CLASS<cl::Device> devices, cl_context_properties* cps);
	static cl::CommandQueue createCommandQueue(cl::Context context, cl::Device device);

	static VECTOR_CLASS<cl::Device> getOnlyValidDevices(VECTOR_CLASS<cl::Device> devices);
};

/**
 * \brief Utilities for working with OpenCL
 *
 * NOTE: Based on Thunder
 * NOTE: Written using OpenCL 1.1
 *
 * \date Dec 10, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class OpenCLUtilities
{
public:
	static void generateOpenCLError(cl_int id, const char* file, int line);
	static std::string getDeviceString(cl_device_id dev);
	static char * file2string(const char* filename, size_t * final_length = NULL);
};

#define check_error(id) 													\
{																				\
	if (id != CL_SUCCESS) 														\
	{																			\
		OpenCLUtilities::generateOpenCLError(id, __FILE__, __LINE__);			\
	}																			\
}

}
#endif /* CXOPENCLUTILITIES_H_ */

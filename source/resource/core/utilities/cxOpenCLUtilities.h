#ifndef CXOPENCLUTILITIES_H_
#define CXOPENCLUTILITIES_H_

#ifdef SSC_USE_OpenCL

#if defined(__APPLE__) || defined(__MACOSX)
    #include "OpenCL/cl.hpp"
#else
    #include <CL/cl.hpp>
#endif


#include <string>

class QString;

namespace cx
{
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
	struct ocl_context
	{
		cl_context context;
		cl_device_id device;
		cl_command_queue cmd_queue;
	};

	//static OpenCL::ocl_context* init(QString processorType);
	static ocl_context* ocl_init(QString processor);
	static void ocl_release(ocl_context* context);
	static cl_kernel ocl_kernel_build(cl_program program, cl_device_id device, const char * kernel_name);
	static cl_mem ocl_create_buffer(cl_context context, cl_mem_flags flags, size_t size, void * host_data);
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
	static std::string ocl_get_device_string(cl_device_id dev);
	static char * file2string(const char* filename, size_t * final_length = NULL);
};

#define ocl_check_error(id) 													\
{																				\
	if (id != CL_SUCCESS) 														\
	{																			\
		OpenCLUtilities::generateOpenCLError(id, __FILE__, __LINE__);			\
	}																			\
}

/**
 * \brief Utilities for getting information about OpenCL
 *
 * NOTE: Written using OpenCL 1.1
 * WARNING: the print commands might not contain all available information.
 *
 * \date Dec 9, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class OpenCLInfo
{
public:
	static void printPlatformAndDeviceInfo();
	static void printPlatformInfo(cl::Platform platform);
	static void printDeviceInfo(cl::Device device);

	static void printDeviceInfo(cl_device_id device, unsigned int indentTimes = 1, bool verbose = false);

	static void printContextInfo(cl_context context, unsigned int indentTimes = 1);
	static void printProgramInfo(cl_program program, unsigned int indentTimes = 1, bool printSource = false);
	static void printProgramBuildInfo(cl_program program, cl_device_id device, unsigned int indentTimes = 1);
	static void printKernelInfo(cl_kernel kernel, unsigned int indentTimes = 1);
	static void printKernelWorkGroupInfo(cl_kernel kernel, cl_device_id device, unsigned int indentTimes = 1);
	static void printCommandQueueInfo(cl_command_queue command_queue, unsigned int indentTimes = 1);
	static void printMemInfo(cl_mem memobj, unsigned int indentTimes = 1);
	static void printSamplerInfo(cl_sampler sampler, unsigned int indentTimes = 1); //untested
	static void printEventInfo(cl_event event, unsigned int indentTimes = 1); //untested

private:
	static void printCharList(const char* list, const char* separator, const char* indentation);
	static void printStringList(std::string list, std::string separator = " ");
	static void print(std::string name, std::string value, int indents = 1);
	static void print(std::string name, int value, int indents = 1);
	static std::string const getIndentation(unsigned int numberOfIndents);
};
}
#endif //SSC_USE_OpenCL
#endif /* CXOPENCLUTILITIES_H_ */

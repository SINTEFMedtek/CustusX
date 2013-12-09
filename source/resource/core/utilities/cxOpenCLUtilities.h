#ifndef CXOPENCLUTILITIES_H_
#define CXOPENCLUTILITIES_H_

#ifdef SSC_USE_OpenCL

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif //__APPLE__

#include <string>

namespace cx
{
/**
 * \brief Utilities for working with OpenCL
 *
 * WARNING: the print commands might not contain all available information.
 *
 * \date Dec 9, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class OpenCLUtilities
{
public:
	static void printPlatformAndDeviceInfo();

	static void printPlatformInfo(cl_platform_id platform, unsigned int indentTimes = 1);
	static void printDeviceInfo(cl_device_id device, unsigned int indentTimes = 1);
	static void printContextInfo(cl_context context, unsigned int indentTimes = 1);
	static void printProgramInfo(cl_program program, unsigned int indentTimes = 1, bool printSource = false);
	static void printKernelInfo(cl_kernel kernel, unsigned int indentTimes = 1);

private:
	static void printCharList(const char* list, const char* separator, const char* indentation);
	static std::string getIndentation(unsigned int numberOfIndents);
};
}
#endif //SSC_USE_OpenCL
#endif /* CXOPENCLUTILITIES_H_ */

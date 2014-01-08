#ifndef CXOPENCLPRINTER_H_
#define CXOPENCLPRINTER_H_

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
 * \brief Utilities for printing information about OpenCL
 *
 * NOTE: Written using OpenCL 1.1
 * WARNING: the print commands might not contain all available information.
 *
 * \date Dec 9, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class OpenCLPrinter
{
public:
	static void printPlatformAndDeviceInfo();
	static void printPlatformInfo(cl::Platform platform);
	static void printDeviceInfo(cl::Device device, bool verbose = false);

private:
	static void printStringList(std::string list, std::string separator = " ");
	static void print(std::string name, std::string value, int indents = 1);
	static void print(std::string name, int value, int indents = 1);
	static std::string const getIndentation(unsigned int numberOfIndents);
};
} //namespace cx
#endif //SSC_USE_OpenCL
#endif /* CXOPENCLPRINTER_H_ */

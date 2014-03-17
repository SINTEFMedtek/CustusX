#ifndef CXOPENCLPRINTER_H_
#define CXOPENCLPRINTER_H_

#include <string>
#include "OpenCLManager.hpp"
class QString;

namespace cx
{
/**
 * \brief Utilities for printing information about OpenCL
 *
 * NOTE: Written using OpenCL 1.1
 * WARNING: the print commands might not contain all available information.
 *
 * \ingroup cx_resource_core_utilities
 * \date Dec 9, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class OpenCLPrinter
{
public:
	static void printPlatformAndDeviceInfo();
	static void printPlatformInfo(cl::Platform platform);
	static void printDeviceInfo(cl::Device device, bool verbose = false);
	static void printContextInfo(cl::Context context);
	static void printProgramInfo(cl::Program program);
	static void printProgramSource(cl::Program program);
	static void printKernelInfo(cl::Kernel kernel);
	static void printMemoryInfo(cl::Memory memory);

private:
	static void printStringList(std::string list, std::string separator = " ");
	static void print(std::string name, std::string value, int indents = 1);
	static void print(std::string name, int value, int indents = 1);
	static std::string const getIndentation(unsigned int numberOfIndents);
};
} //namespace cx
#endif /* CXOPENCLPRINTER_H_ */

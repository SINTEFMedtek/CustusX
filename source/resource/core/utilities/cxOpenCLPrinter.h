/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXOPENCLPRINTER_H_
#define CXOPENCLPRINTER_H_

#include "cxResourceExport.h"

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
class cxResource_EXPORT OpenCLPrinter
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

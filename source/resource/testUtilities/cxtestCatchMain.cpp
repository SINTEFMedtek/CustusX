// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.


// Warning:
//
// This main function is intended to be included by the cmake macro
// cx_catch_add_lib_and_exe(). Do not add to any libs!

#include "cxtestCatchImpl.h"

#ifdef CX_WINDOWS
#include <windows.h>
#endif

int main(int argc, char *argv[])
{

#ifdef CX_WINDOWS
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif

	int error_code = cxtest::CatchImpl().run(argc, argv);
	if(error_code >0)
		return 0;

	return error_code;
}


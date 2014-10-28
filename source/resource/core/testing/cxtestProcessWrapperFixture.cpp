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

#include "cxtestProcessWrapperFixture.h"

#include "cxReporter.h"

namespace cxtest {

ProcessWrapperFixture::ProcessWrapperFixture()
{
	cx::Reporter::initialize();
}

ProcessWrapperFixture::~ProcessWrapperFixture()
{
	cx::Reporter::shutdown();
}

bool ProcessWrapperFixture::canLaunchGit_Version()
{
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());

#if defined CX_WINDOWS
    //git should be available in the run environment of Catch
    exe->launch("git", QStringList("--version"));
#else
	QString location1 = "/usr/bin/git";
	QString location2 = "/opt/local/bin/git";
	if(QFile::exists(location1))
		exe->launch(location1, QStringList("--version"));
	else if(QFile::exists(location2))
		exe->launch(location2, QStringList("--version"));
#endif

	return getResultFromFinishedExecution(exe);
}

bool ProcessWrapperFixture::canLaunchVLC_Version()
{
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());

#if defined CX_WINDOWS
	//Warning: this produces a popup that won't close...
	exe->launch("\"C:/Program Files (x86)/VideoLAN/VLC/vlc.exe\" --version");
#elif defined CX_LINUX
	exe->launch("/usr/bin/vlc", QStringList("--version"));
#elif defined CX_APPLE
	exe->launch("/Applications/VLC.app/Contents/MacOS/VLC", QStringList("--version"));
#endif

	return getResultFromFinishedExecution(exe);
}

bool ProcessWrapperFixture::canLaunchNotExistingExecutable()
{
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());
	exe->launch("NotExistingExecutable");

	return getResultFromFinishedExecution(exe);
}

bool ProcessWrapperFixture::getResultFromFinishedExecution(cx::ProcessWrapperPtr exe)
{
	if(!exe->waitForStarted())
		return false;

	if(!exe->waitForFinished())
		return false;

	return true;
}

} /* namespace cxtest */

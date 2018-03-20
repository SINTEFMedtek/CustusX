/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestProcessWrapperFixture.h"
#include "cxLogger.h"
#include <QFile>
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

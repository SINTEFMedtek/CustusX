#include "cxtestProcessWrapperFixture.h"

#include "sscMessageManager.h"

namespace cxtest {

ProcessWrapperFixture::ProcessWrapperFixture()
{
	cx::MessageManager::initialize();
}

ProcessWrapperFixture::~ProcessWrapperFixture()
{
	cx::MessageManager::shutdown();
}

bool ProcessWrapperFixture::canLaunchGit_Version()
{
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());

#if defined CX_WINDOWS
	exe->launch("\"C:\\Program Files (x86)\\Git\\cmd\\git.cmd\"", QStringList("--version"));
#elif defined CX_LINUX
	exe->launch("/usr/bin/git", QStringList("--version"));
#elif defined CX_APPLE
	exe->launch("/opt/local/bin/git", QStringList("--version"));
#endif

	return getResultFromFinishedExecution(exe);
}

bool ProcessWrapperFixture::canLaunchVLC_Version()
{
	cx::ProcessWrapperPtr exe(new cx::ProcessWrapper());

#if defined CX_WINDOWS
	exe->launch("\"C:\\Program Files (x86)\\Git\\cmd\\git.cmd\"", QStringList("--version"));
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
	if(!exe->getProcess()->waitForStarted())
		return false;

	if(!exe->getProcess()->waitForFinished())
		return false;

	return true;
}

} /* namespace cxtest */

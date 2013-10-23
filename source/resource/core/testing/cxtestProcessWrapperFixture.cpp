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
	exe->launch("\"C:\Program Files (x86)\VideoLAN\VLC\vlc.exe\"", QStringList("--version"));
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

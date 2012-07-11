//#include <stdarg.h>
//#include <stdio.h>
//#include <sys/time.h>
//#include <sys/resource.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <time.h>
#ifndef WIN32
	#include <syslog.h>
#endif
//#include <unistd.h>

#include <sscMessageManager.h>
#include <QString>
#include <QFileInfo>
#include <QDir>

//#include "../../snwDefines.h"
#include "sscLogger.h"

#define MAX_LEN_LOG_LINE 500
#define SW_UMASK ( S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH )

//static bool loggingEnabled = true;
//static bool initialized = false;

/** Produces logs in the cwd.
 *
 */
int SSC_Logging_Init_Default(const char *busName)
{
	return SSC_Logging_Init(busName, NULL);
}

int SSC_Logging_Init(const char *logName, const char* applicationPath)
{
	QString basePath(applicationPath);
	if (basePath.isEmpty())
	{
		basePath = ".";
	}

	if ( !logName )
	{
		SSC_ERROR( "No log name specified!" );
		return -1;
	}

	QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	QString logFile = basePath + "/Logs/" + timestamp + "/" + QString(logName) + ".txt";

	#ifndef WIN32
	openlog( NULL, LOG_CONS | LOG_PERROR, LOG_USER );
	#endif

	ssc::MessageManager::initialize(); // initialize
	ssc::messageManager()->setLogFile(logFile);

	return 0;
}

void SSC_Logging_Done( void )
{
	ssc::MessageManager::shutdown();
	#ifndef WIN32
		closelog();
	#endif
}

void SSC_Logging( bool on )
{
	ssc::messageManager()->setEnabled(on);
}

namespace // unnamed
{
	QString mergeSourceInfo(const char *file, int line, const char *function)
	{
		QString filename = QFileInfo(file).fileName();
		return QString("%1:%2/%3").arg(filename).arg(line).arg(function);
	}
}


void SSC_Log( const char *file, int line, const char *function, const char *format, ... )
{
	if (!ssc::messageManager()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end(ap);

	ssc::messageManager()->sendMessage(buf, ssc::mlINFO, 0, false, mergeSourceInfo(file, line, function));
}

void SSC_Error( const char *file, int line, const char *function, const char *format, ... )
{
	if (!ssc::messageManager()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end( ap );

	#ifndef WIN32
	syslog(LOG_CRIT | LOG_USER, "%s: %s", function, buf);
	#endif

	ssc::messageManager()->sendMessage(buf, ssc::mlERROR, 0, false, mergeSourceInfo(file, line, function));
}

void SSC_Warning( const char *file, int line, const char *function, const char *format, ... )
{
	if (!ssc::messageManager()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end( ap );

	#ifndef WIN32
	syslog(LOG_WARNING | LOG_USER, "%s : %s", function, buf);
	#endif
	ssc::messageManager()->sendMessage(buf, ssc::mlWARNING, 0, false, mergeSourceInfo(file, line, function));
}


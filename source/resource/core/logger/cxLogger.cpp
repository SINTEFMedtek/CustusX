#include "cxReporter.h"
#include <QFileInfo>
#include "cxLogger.h"

#define MAX_LEN_LOG_LINE 500


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
	if (!cx::reporter()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end(ap);

	cx::reporter()->sendMessage(buf, cx::mlINFO, 0, false, mergeSourceInfo(file, line, function));
}

void SSC_Error( const char *file, int line, const char *function, const char *format, ... )
{
	if (!cx::reporter()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end( ap );

	cx::reporter()->sendMessage(buf, cx::mlERROR, 0, false, mergeSourceInfo(file, line, function));
}

void SSC_Warning( const char *file, int line, const char *function, const char *format, ... )
{
	if (!cx::reporter()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end( ap );

	cx::reporter()->sendMessage(buf, cx::mlWARNING, 0, false, mergeSourceInfo(file, line, function));
}


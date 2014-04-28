#include "cxUtilHelpers.h"
#include <QStringList>
#include <unistd.h>

namespace cx
{

/**Constrain the input to the range |min,max|
 */
double constrainValue(double val, double min, double max)
{
	if (val<=min)
		return min;
	if (val>=max)
		return max;
	return val;
}

int sign(double x)
{
	if (x>=0)
		return 1;
	return -1;
}

QString changeExtension(QString name, QString ext)
{
	QStringList splitName = name.split(".");

	if (splitName.size()==1)
		return name;

	splitName[splitName.size()-1] = ext;

	if (ext.isEmpty())
	{
		splitName.pop_back();
	}

	return splitName.join(".");
}

void sleep_ms(int ms)
{
#ifndef CX_WINDOWS
		usleep(ms*1000);
#else
		Sleep(ms);
#endif
}

} // namespace cx


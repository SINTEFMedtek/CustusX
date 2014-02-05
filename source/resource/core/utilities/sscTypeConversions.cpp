#include "sscTypeConversions.h"
#include <QStringList>
#include <iostream>
#include "sscVector3D.h"

template<> cstring_cast_Placeholder cstring_cast<QString>(const QString& val)
{
	return cstring_cast_Placeholder(val);
}
template<> cstring_cast_Placeholder cstring_cast<QVariant>(const QVariant& val)
{
	return cstring_cast_Placeholder(val.toString());
}

/** Helper function overload for streaming a QString to std::cout.
 */
std::ostream& operator<<(std::ostream& str, const QString& qstring)
{
	str << qstring.toStdString();
	return str;
}

std::vector<double> convertQString2DoubleVector(const QString& input, bool* ok)
{
	if (ok)
		*ok = true;
	QStringList comp = input.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	std::vector<double> retval(comp.size());
	bool tempOk = true;

	for (unsigned i=0; i<retval.size(); ++i)
	{
		retval[i] = comp[i].toDouble(&tempOk);
		if (ok)
			*ok = *ok && tempOk;
	}
	return retval;
}


QString color2string(QColor color)
{
	return qstring_cast(Eigen::Vector4f(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
}


QColor string2color(QString input, QColor defaultValue)
{
	QStringList c = input.split(" ");
	if (c.size()<4)
		return defaultValue;
	bool ok;
	return QColor::fromRgbF(c[0].toDouble(&ok), c[1].toDouble(&ok), c[2].toDouble(&ok), c[3].toDouble(&ok));
}




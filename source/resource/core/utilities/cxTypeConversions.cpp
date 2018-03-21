/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTypeConversions.h"
#include <QStringList>
#include <iostream>
#include "cxVector3D.h"

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
	QString retval = QString("%1/%2/%3")
					.arg(color.red())
					.arg(color.green())
					.arg(color.blue());
	if (color.alpha()!=255)
		retval += QString("/%1").arg(color.alpha());
	return retval;
}


QColor string2color(QString input, QColor defaultValue)
{
	QStringList c = input.split("/");
	if (c.size()==3)
		c.push_back("255");
	if (c.size()<4)
		return defaultValue;
	bool ok;
	QColor retval = QColor::fromRgb(
				c[0].toInt(&ok),
				c[1].toInt(&ok),
				c[2].toInt(&ok),
				c[3].toInt(&ok));
	if (!ok)
		return defaultValue;
	return retval;
}




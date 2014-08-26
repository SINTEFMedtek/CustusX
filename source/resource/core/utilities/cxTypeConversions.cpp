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




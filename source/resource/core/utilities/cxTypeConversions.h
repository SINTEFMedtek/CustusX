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

#ifndef CXTYPECONVERSIONS_H_
#define CXTYPECONVERSIONS_H_

#include "cxResourceExport.h"

#include <string>
#include <vector>
#include <QString>
#include <QVariant>
#include <QDomDocument>
#include <QColor>

#ifndef Q_MOC_RUN // workaround for bug in moc vs boost 1.48+: https://bugreports.qt-project.org/browse/QTBUG-22829
#include <boost/lexical_cast.hpp>
#endif

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

/**Convenience function that converts a type to its 
 * string representation, provided it has defined operator <<.
 */
template<class T>
std::string string_cast(const T& val)
{
	return boost::lexical_cast<std::string>(val);
}

/**Convenience function that converts a type to its 
 * QString representation, provided it has defined operator <<.
 */
template<class T>
QString qstring_cast(const T& val)
{
	return QString::fromStdString(string_cast(val));
}

/**utitity class for the cstring_cast function
 */
class cxResource_EXPORT cstring_cast_Placeholder
{
public:
	explicit cstring_cast_Placeholder(const QString& val) : mData(val.toStdString()) {}
	operator const char*() const { return mData.c_str(); }
	operator char*() const { return const_cast<char*>(mData.c_str()); } // use with extreme caution.
	const char* c() const { return mData.c_str(); }
private:
	std::string mData;
};

/**Convenience function that converts a type to its 
 * char* representation, provided it has defined operator <<
 * or is QString or QVariant
 */
template<class T>
cstring_cast_Placeholder cstring_cast(const T& val)
{
	return cstring_cast_Placeholder(string_cast(val));
}
template<> cxResource_EXPORT cstring_cast_Placeholder cstring_cast<QString>(const QString& val);
template<> cxResource_EXPORT cstring_cast_Placeholder cstring_cast<QVariant>(const QVariant& val);

/** Helper function overload for streaming a QString to std::cout.
 */
cxResource_EXPORT std::ostream& operator<<(std::ostream& str, const QString& qstring);

/** Helper function for converting a QString to a list of doubles.
 *  Useful for reading vectors/matrices.
 */
cxResource_EXPORT std::vector<double> convertQString2DoubleVector(const QString& input, bool* ok=0);

/** Helper function template for streaming an object to string.
 *  The streamed object must support the method void addXml(QDomNode).
 */
template<class T>
QString streamXml2String(T& val)
{
	QDomDocument doc;
	QDomElement root = doc.createElement("root");
	doc.appendChild(root);

	val.addXml(root);
	return doc.toString();
}
template<> cxResource_EXPORT QString streamXml2String(QString& val);

cxResource_EXPORT QString color2string(QColor color);
cxResource_EXPORT QColor string2color(QString input, QColor defaultValue=QColor("green"));

/**
 * \}
 */

#endif /*CXTYPECONVERSIONS_H_*/

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxDefinitions.h"

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

namespace cx
{
QString convertToReadableString(QString string);
QString convertToReadableString(ORGAN_TYPE target);
}//cx

/**
 * \}
 */

#endif /*CXTYPECONVERSIONS_H_*/

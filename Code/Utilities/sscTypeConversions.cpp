#include "sscTypeConversions.h"


template<> cstring_cast_Placeholder cstring_cast<QString>(const QString& val)
{
	return cstring_cast_Placeholder(val.toStdString());
}
template<> cstring_cast_Placeholder cstring_cast<QVariant>(const QVariant& val)
{
	return cstring_cast_Placeholder(val.toString().toStdString());
}

/** Helper function overload for streaming a QString to std::cout.
 */
std::ostream& operator<<(std::ostream& str, const QString& qstring)
{
	str << qstring.toStdString();
	return str;
}



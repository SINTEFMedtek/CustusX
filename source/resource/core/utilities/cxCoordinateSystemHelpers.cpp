#include "cxCoordinateSystemHelpers.h"

//#include "cxToolManager.h"
//#include "cxReporter.h"
//#include "cxDataManager.h"
//#include "cxData.h"
#include "cxDefinitionStrings.h"
//#include "cxImage.h"
//#include "vtkImageData.h"

namespace cx
{

QString CoordinateSystem::toString() const
{
	return enum2string(mId) + (mRefObject.isEmpty() ? "" : ("/"+mRefObject));
}

CoordinateSystem CoordinateSystem::fromString(QString text)
{
	QStringList raw = text.split("/");
	CoordinateSystem retval(csCOUNT);
	if (raw.size()<1)
		return retval;
	retval.mId = string2enum<COORDINATE_SYSTEM>(raw[0]);
	if (raw.size()<2)
		return retval;
	retval.mRefObject = raw[1];
	return retval;
}

bool operator==(const CoordinateSystem& lhs, const CoordinateSystem& rhs)
{
	return ( lhs.mId==rhs.mId )&&( lhs.mRefObject==rhs.mRefObject );
}

// --------------------------------------------------------

} //namespace cx

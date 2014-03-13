#ifndef SSCTESTUTILITIES_H_
#define SSCTESTUTILITIES_H_

#include <string>
#include <QString>
#include "sscConfig.h"

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

struct TestUtilities
{
	static inline QString GetDataRoot();
	static inline QString ExpandDataFileName(QString fileName);
};

inline
QString TestUtilities::GetDataRoot()
{
	QString dataRoot = SSC_DATA_ROOT;
	return dataRoot;
}

inline
QString TestUtilities::ExpandDataFileName(QString fileName)
{
	QString dataRoot = SSC_DATA_ROOT;
	QString expandedName = dataRoot + fileName;
	return expandedName;
}

/**
 * \}
 */

} // namespace cx

#endif /*SSCTESTUTILITIES_H_*/

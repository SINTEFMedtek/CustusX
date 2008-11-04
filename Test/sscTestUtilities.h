#ifndef SSCTESTUTILITIES_H_
#define SSCTESTUTILITIES_H_

#include <string>

#include "sscConfig.h"

namespace ssc
{

struct TestUtilities
{
	static inline std::string GetDataRoot();
	static inline std::string ExpandDataFileName(std::string fileName);
};

inline
std::string TestUtilities::GetDataRoot()
{
	std::string dataRoot = SSC_DATA_ROOT;
	return dataRoot;
}

inline
std::string TestUtilities::ExpandDataFileName(std::string fileName)
{
	std::string dataRoot = SSC_DATA_ROOT;
	std::string expandedName = dataRoot + fileName;
	return expandedName;
}


} // namespace ssc

#endif /*SSCTESTUTILITIES_H_*/

#ifndef CXOPENCLUTILITIES_H_
#define CXOPENCLUTILITIES_H_

#ifdef SSC_USE_OpenCL

namespace cx
{
/**
 * \brief Utilities for working with OpenCL
 *
 * \date Dec 9, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class OpenCLUtilities
{
public:
	OpenCLUtilities();

	static void printDeviceInfo();
};
}
#endif //SSC_USE_OpenCL
#endif /* CXOPENCLUTILITIES_H_ */

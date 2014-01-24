#ifndef CXTESTOPENCLFIXTURE_H_
#define CXTESTOPENCLFIXTURE_H_

#include "cxOpenCLUtilities.h"

namespace cxtest
{
/**
 * \brief 
 *
 * \date Jan 23, 2014
 * \author Janne Beate Bakeng, SINTEF
 */

class OpenCLFixture
{
public:
	OpenCLFixture();
	~OpenCLFixture();

	bool runTestKernel(cl_device_type type);

private:
	void waitForOpenCLToShutDown();

};

} /* namespace cx */

#endif /* CXTESTOPENCLFIXTURE_H_ */

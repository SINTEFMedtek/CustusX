#ifndef CXTESTUTILITIES_H_
#define CXTESTUTILITIES_H_

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

namespace cxtest
{

/**
 * A collection of functionality used for testing.
 *
 * \date May 21, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class Utilities
{
public:
	static vtkImageDataPtr create3DVtkImageData();
	static ssc::ImagePtr create3DImage();
};

} /* namespace cxtest */
#endif /* CXTESTUTILITIES_H_ */

// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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

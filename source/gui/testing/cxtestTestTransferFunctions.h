/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTTESTTRANSFERFUNCTIONS_H_
#define CXTESTTESTTRANSFERFUNCTIONS_H_

#include "cxtestgui_export.h"

#include <vtkImageData.h>
#include "cxImage.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"

namespace cxtest {

/*
 * TestTransferFunctions
 *
 * \brief Helper class for testing 2D/3D transfer functions
 *
 * \date Jul 31, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class CXTESTGUI_EXPORT TestTransferFunctions {
public:
	TestTransferFunctions();
	void Corrupt3DTransferFunctionWindowWidth();
	void Corrupt3DTransferFunctionWindowLevel();
	void Corrupt2DTransferFunctionWindowWidth();
	void Corrupt2DTransferFunctionWindowLevel();
	void setNewCorrupt3DTranferFunction();
	void setNewCorrupt2DTranferFunction();

private:
	vtkImageDataPtr mDummyImageData;
	cx::ImagePtr mDummyImage;

	void createDummyImage();
};

} /* namespace cxtest */
#endif /* CXTESTTESTTRANSFERFUNCTIONS_H_ */

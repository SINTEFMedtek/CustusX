/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXTESTIGTLINKCONVERSIONFIXTURE_H_
#define CXTESTIGTLINKCONVERSIONFIXTURE_H_

#include "cxtestopenigtlinkutilities_export.h"


#include "cxImage.h"
#include "cxIGTLinkImageMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkConversion.h"
#include "cxVolumeHelpers.h"
#include "vtkImageData.h"


using namespace cx;

class CXTESTOPENIGTLINKUTILITIES_EXPORT IGTLinkConversionFixture
{
protected:
	typedef std::vector<std::pair<Eigen::Array3i, Eigen::Array3i> > Val3VectorType;

	/** Create a 2D RGBA test image with some variation
	 *
	 */
    vtkImageDataPtr createRGBATestImage();

    int getValue(cx::ImagePtr data, int x, int y, int z);

    Eigen::Array3i getValue3i(cx::ImagePtr data, int x, int y, int z);

    void setValue(vtkImageDataPtr data, int x, int y, int z, unsigned char val);

    void testDecodeEncodeColorImage(Val3VectorType values, QString colorFormat);
};


#endif /* CXTESTIGTLINKCONVERSIONFIXTURE_H_ */

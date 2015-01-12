/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

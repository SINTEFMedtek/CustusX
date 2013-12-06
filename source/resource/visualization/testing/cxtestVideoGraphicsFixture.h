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
#ifndef CXTESTVIDEOGRAPHICSFIXTURE_H_
#define CXTESTVIDEOGRAPHICSFIXTURE_H_

#include "sscTransform3D.h"

#include "sscVideoGraphics.h"
#include "sscProbeData.h"
#include "cxtestRenderTester.h"

namespace cxtest
{

/** Tests for class VideoGraphics
  *
  * \date april 29, 2013
  * \author christiana
  */
class VideoGraphicsFixture
{
public:
	VideoGraphicsFixture();

protected:
	void renderImageAndCompareToExpected(vtkImageDataPtr input, vtkImageDataPtr expected);
	vtkImageDataPtr  readImageData(QString filename, QString description);
	cx::ProbeDefinition readProbeData(QString filename);
	void addImageToRenderer(vtkImageDataPtr image);
	cxtest::RenderTesterPtr mMachine;
	cx::VideoGraphicsPtr mVideoGraphics;
};

} /* namespace cxtest */

#endif /* CXTESTVIDEOGRAPHICSFIXTURE_H_ */

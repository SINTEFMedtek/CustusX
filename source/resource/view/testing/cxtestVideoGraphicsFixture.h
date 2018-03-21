/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTESTVIDEOGRAPHICSFIXTURE_H_
#define CXTESTVIDEOGRAPHICSFIXTURE_H_

#include "cxtestresourcevisualization_export.h"

#include "cxTransform3D.h"

#include "cxVideoGraphics.h"
#include "cxProbeDefinition.h"
#include "cxtestRenderTester.h"

namespace cxtest
{

/** Tests for class VideoGraphics
  *
  * \date april 29, 2013
  * \author christiana
  */
class CXTESTRESOURCEVISUALIZATION_EXPORT VideoGraphicsFixture
{
public:
	VideoGraphicsFixture();

protected:
	void renderImageAndCompareToExpected(vtkImageDataPtr input, vtkImageDataPtr expected);
	vtkImageDataPtr  readImageData(QString filename, QString description);
	cx::ProbeDefinition readProbeDefinition(QString filename);
	void addImageToRenderer(vtkImageDataPtr image);
	cxtest::RenderTesterPtr mMachine;
	cx::VideoGraphicsPtr mVideoGraphics;
};

} /* namespace cxtest */

#endif /* CXTESTVIDEOGRAPHICSFIXTURE_H_ */

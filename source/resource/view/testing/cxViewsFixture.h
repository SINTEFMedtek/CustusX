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
#ifndef CXVIEWSFIXTURE_H
#define CXVIEWSFIXTURE_H

#include "cxtestresourcevisualization_export.h"

#include "cxtestVisServices.h"
#include "cxMessageListener.h"
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"
#include <vector>

namespace cxtest
{
typedef boost::shared_ptr<class RenderTester> RenderTesterPtr;

/**
  * @brief Creates and returns a lut, based on range parameters.
  *
  * @return Pointer to a complete vtkLookupTable.
  */
vtkLookupTablePtr getCreateLut(int tableRangeMin, int tableRangeMax, double hueRangeMin, double hueRangeMax,
	double saturationRangeMin = 0, double saturationRangeMax = 1, double valueRangeMin = 1, double valueRangeMax = 1);

class CXTESTRESOURCEVISUALIZATION_EXPORT ImageParameters
{
public:
	ImageParameters() : llr(.1), alpha(1) {}

	double llr;
	double alpha;
};

typedef boost::shared_ptr<class ViewsWindow> ViewsWindowPtr;

/** Test class  with convenience methods for defining views.
 */
class CXTESTRESOURCEVISUALIZATION_EXPORT ViewsFixture : public QObject
{
	Q_OBJECT

public:
	ViewsFixture(QString displayText="");
	virtual ~ViewsFixture();

	void define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const QString&    imageFilename, cx::PLANE_TYPE plane, int r, int c);
	bool defineGPUSlice(const QString& uid, const std::vector<cx::ImagePtr> images, cx::PLANE_TYPE plane, int r, int c);

	cx::ViewPtr addView(int row, int col);

	bool quickRunWidget();
	bool runWidget();
	void clear();

	double getFractionOfBrightPixelsInRenderWindowForView(int viewIndex, int threshold, int component=0);
	void dumpDebugViewToDisk(QString text, int viewIndex);

	cx::ImagePtr loadImage(const QString& imageFilename);
	cx::DummyToolPtr dummyTool();

private:
	void applyParameters(cx::ImagePtr image, const ImageParameters* parameters);
	void fixToolToCenter();
	cx::SliceProxyPtr createSliceProxy(cx::PLANE_TYPE plane);
	RenderTesterPtr getRenderTesterForRenderWindow(int viewIndex);

	QString mShaderFolder;
	cxtest::TestVisServicesPtr mServices;
	cx::MessageListenerPtr mMessageListener;
	ViewsWindowPtr mWindow;
	cx::RenderWindowFactoryPtr mFactory;
};

} // namespace cxtest


#endif // CXVIEWSFIXTURE_H

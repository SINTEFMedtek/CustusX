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

#ifndef CXTESTRENDERTESTER_H
#define CXTESTRENDERTESTER_H

#include "cxtestresourcevisualization_export.h"

#include <vtkForwardDeclarations.h>
#include <boost/shared_ptr.hpp>
#include <QString>
#include "cxRep.h"
#include "cxView.h"
#include "cxViewCollectionWidget.h"

typedef vtkSmartPointer<class vtkProp> vtkPropPtr;

namespace cxtest
{

typedef boost::shared_ptr<class RenderTester> RenderTesterPtr;

/** Test actor by rendering and comparing the result to a reference image
	*
	* \date april 29, 2013
	* \author christiana
	*/
class CXTESTRESOURCEVISUALIZATION_EXPORT RenderTester
{
public:
	static RenderTesterPtr create();
	static RenderTesterPtr create(vtkRenderWindowPtr renderWindow);
	static RenderTesterPtr create(vtkRenderWindowPtr renderWindow, vtkRendererPtr renderer);

	RenderTester();
	RenderTester(vtkRenderWindowPtr renderWindow);
	RenderTester(vtkRenderWindowPtr renderWindow, vtkRendererPtr renderer);
	void addProp(vtkPropPtr prop);
	void renderToFile(QString filename);

	void resetCamera();
	void alignRenderWindowWithImage(vtkImageDataPtr input);

	void writeToPNG(vtkImageDataPtr image, QString filename);
	vtkImageDataPtr readFromPNG(QString filename);
	vtkImageDataPtr readFromFile(QString filename);
	vtkImageDataPtr renderToImage();
	bool findDifference(vtkImageDataPtr input1, vtkImageDataPtr input2);
	void enterRunLoop();
	void setImageErrorThreshold(double value);
	void printFractionOfVoxelsAboveZero(QString desc, vtkImageDataPtr image);
	vtkImageDataPtr getImageFromRenderWindow();

	void renderAndUpdateText(int num);
	void addTextToVtkRenderWindow(QString text);

private:
	vtkImageDataPtr clipImage(vtkImageDataPtr input);
	bool equalExtent(vtkImageDataPtr input1, vtkImageDataPtr input2);
	bool hasValidDimensions(vtkImageDataPtr input);
	vtkImageDataPtr convertToColorImage(vtkImageDataPtr image);
	bool equalNumberOfComponents(vtkImageDataPtr image1, vtkImageDataPtr image2);

	vtkRenderWindowPtr mRenderWindow;
	vtkRendererPtr mRenderer;
	double mImageErrorThreshold;
	int mBorderOffset;

	vtkTextMapperPtr mMapper;
};

} // namespace cxtest


#endif // CXTESTRENDERTESTER_H

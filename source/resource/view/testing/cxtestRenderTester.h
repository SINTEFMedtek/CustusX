/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

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
#ifndef SSCVTKRENDERTESTER_H
#define SSCVTKRENDERTESTER_H

#include <vtkForwardDeclarations.h>
#include "boost/shared_ptr.hpp"
#include <QString>

typedef vtkSmartPointer<class vtkProp> vtkPropPtr;

namespace ssc
{


typedef boost::shared_ptr<class TestVtkRendering> TestVtkRenderingPtr;

/** Test actor by rendering and comparing the result to a reference image
  *
  * \date april 29, 2013
  * \author christiana
  */
class TestVtkRendering
{
public:
	static TestVtkRenderingPtr create();

	TestVtkRendering();
//	ssc::ViewWidget* getView();
	void addProp(vtkPropPtr prop);
	void renderToFile(QString filename);

	void resetCamera();
	void alignRenderWindowWithImage(vtkImageDataPtr input);

	vtkImageDataPtr readFromPNG(QString filename);
	vtkImageDataPtr readFromFile(QString filename);
	vtkImageDataPtr renderToImage();
	bool findDifference(vtkImageDataPtr input1, vtkImageDataPtr input2);
	void enterRunLoop();

private:
	vtkImageDataPtr getImageFromRenderWindow();
	void writeToPNG(vtkImageDataPtr image, QString filename);
	vtkImageDataPtr clipImage(vtkImageDataPtr input);
	bool equalExtent(vtkImageDataPtr input1, vtkImageDataPtr input2);
	bool hasValidDimensions(vtkImageDataPtr input);
	vtkImageDataPtr convertToColorImage(vtkImageDataPtr image);
	bool equalNumberOfComponents(vtkImageDataPtr image1, vtkImageDataPtr image2);

	//	ssc::ViewWidget* mView;
	vtkRenderWindowPtr mRenderWindow;
	vtkRendererPtr mRenderer;
	double mImageErrorThreshold;
	int mBorderOffset;
};

} // namespace ssc

#endif // SSCVTKRENDERTESTER_H

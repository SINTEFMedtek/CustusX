/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestTestTransferFunctionColorWidget.h"

#include <vtkImageData.h>
#include "cxImage.h"
#include "cxImageTF3D.h"

namespace cxtest {

TestTransferFunctionColorWidget::TestTransferFunctionColorWidget(cx::ActiveDataPtr activeData) :
		TransferFunctionColorWidget(activeData, NULL)
{}


void TestTransferFunctionColorWidget::initWithTestData()
{
	vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(2, 1);
	cx::ImagePtr dummyImage(new cx::Image("DummyImage", dummyImageData));
	cx::ImageTFDataPtr transferFunction;
	transferFunction = dummyImage->getTransferFunctions3D();
	this->setData(dummyImage, transferFunction);
}

void TestTransferFunctionColorWidget::setPlotArea(QRect plotArea)
{
	this->mPlotArea = plotArea;
}

int TestTransferFunctionColorWidget::getLeftAreaBoundary()
{
	return mPlotArea.left();
}
int TestTransferFunctionColorWidget::getRigthAreaBoundary()
{
	return mPlotArea.right();
}

} /* namespace cxtest */

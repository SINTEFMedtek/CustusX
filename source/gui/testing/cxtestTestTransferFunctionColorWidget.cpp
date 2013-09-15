#include "cxtestTestTransferFunctionColorWidget.h"

#include <vtkImageData.h>
#include "sscImage.h"
#include "sscImageTF3D.h"

namespace cxtest {

TestTransferFunctionColorWidget::TestTransferFunctionColorWidget() :
		TransferFunctionColorWidget(NULL),
		mLeftAreaBoundary(0),
		mRightAreaBoundary(0),
		mWidthAreaBoundary(0)
{}


void TestTransferFunctionColorWidget::initWithTestData()
{
	vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(2, 1);
	cx::ImagePtr dummyImage(new cx::Image("DummyImage", dummyImageData));
	cx::ImageTFDataPtr transferFunction;
	transferFunction = dummyImage->getTransferFunctions3D();
	this->setData(dummyImage, transferFunction);
	this->calculateColorTFBoundaries(
			this->mLeftAreaBoundary,
			this->mRightAreaBoundary,
			this->mWidthAreaBoundary);
}

void TestTransferFunctionColorWidget::setPlotArea(QRect plotArea)
{
	this->mPlotArea = plotArea;
}

int TestTransferFunctionColorWidget::getLeftAreaBoundary()
{
	return this->mLeftAreaBoundary;
}
int TestTransferFunctionColorWidget::getRigthAreaBoundary()
{
	return this->mRightAreaBoundary;
}

} /* namespace cxtest */

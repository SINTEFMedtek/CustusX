#include "cxtestTestTransferFunctionColorWidget.h"

#include <vtkImageData.h>
#include "sscImage.h"
#include "sscImageTF3D.h"

namespace cxtest {

TestTransferFunctionColorWidget::TestTransferFunctionColorWidget() :
		TransferFunctionColorWidget(NULL)
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

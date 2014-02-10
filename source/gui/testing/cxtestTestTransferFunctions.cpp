/*
 * cxtestTestTransferFunctions.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: olevs
 */

#include "cxtestTestTransferFunctions.h"

namespace cxtest {

TestTransferFunctions::TestTransferFunctions()
{
	this->createDummyImage();
}

void TestTransferFunctions::createDummyImage()
{
	mDummyImageData = cx::Image::createDummyImageData(2, 1);
	mDummyImage.reset(new cx::Image("DummyImage", this->mDummyImageData));
}

//bool TestTransferFunctions::hasValid3DTransferFunction()
//{
//	return mDummyImage->isValidTransferFunction(mDummyImage->getTransferFunctions3D());
//}

//bool TestTransferFunctions::hasValid2DTransferFunction()
//{
//	return mDummyImage->isValidTransferFunction(mDummyImage->getLookupTable2D());
//}

void TestTransferFunctions::Corrupt3DTransferFunctionWindowWidth()
{
	cx::ImageTFDataPtr transferFunction = mDummyImage->getTransferFunctions3D();
	transferFunction->setWindow(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::Corrupt3DTransferFunctionWindowLevel()
{
	cx::ImageTFDataPtr transferFunction = mDummyImage->getTransferFunctions3D();
	transferFunction->setLevel(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::Corrupt2DTransferFunctionWindowWidth()
{
	cx::ImageTFDataPtr transferFunction = mDummyImage->getLookupTable2D();
	transferFunction->setWindow(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::Corrupt2DTransferFunctionWindowLevel()
{
	cx::ImageTFDataPtr transferFunction = mDummyImage->getLookupTable2D();
	transferFunction->setLevel(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::setNewCorrupt3DTranferFunction()
{
	cx::ImageTF3DPtr transferFunction(new cx::ImageTF3D());
//	transferFunction->setInitialTFFromImage(mDummyImage->getBaseVtkImageData());
	transferFunction->setWindow(mDummyImage->getMax() + 10);
	mDummyImage->setTransferFunctions3D(transferFunction);
}

void TestTransferFunctions::setNewCorrupt2DTranferFunction()
{
	cx::ImageLUT2DPtr transferFunction(new cx::ImageLUT2D());
//	transferFunction->setInitialTFFromImage(mDummyImage->getBaseVtkImageData());
	transferFunction->setWindow(mDummyImage->getMax() + 10);
	mDummyImage->setLookupTable2D(transferFunction);
}

} /* namespace cxtest */

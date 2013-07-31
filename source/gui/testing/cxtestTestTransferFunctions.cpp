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
	mDummyImageData = ssc::Image::createDummyImageData(2, 1);
	mDummyImage.reset(new ssc::Image("DummyImage", this->mDummyImageData));
}

bool TestTransferFunctions::hasValid3DTransferFunction()
{
	return mDummyImage->isValidTransferFunction(mDummyImage->getTransferFunctions3D());
}

bool TestTransferFunctions::hasValid2DTransferFunction()
{
	return mDummyImage->isValidTransferFunction(mDummyImage->getLookupTable2D());
}

void TestTransferFunctions::Corrupt3DTransferFunctionWindowWidth()
{
	ssc::ImageTFDataPtr transferFunction = mDummyImage->getTransferFunctions3D();
	transferFunction->setWindow(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::Corrupt3DTransferFunctionWindowLevel()
{
	ssc::ImageTFDataPtr transferFunction = mDummyImage->getTransferFunctions3D();
	transferFunction->setLevel(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::Corrupt2DTransferFunctionWindowWidth()
{
	ssc::ImageTFDataPtr transferFunction = mDummyImage->getLookupTable2D();
	transferFunction->setWindow(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::Corrupt2DTransferFunctionWindowLevel()
{
	ssc::ImageTFDataPtr transferFunction = mDummyImage->getLookupTable2D();
	transferFunction->setLevel(mDummyImage->getMax() + 10);
}

void TestTransferFunctions::setNewCorrupt3DTranferFunction()
{
	ssc::ImageTF3DPtr transferFunction(new ssc::ImageTF3D(mDummyImageData));
	transferFunction->setWindow(mDummyImage->getMax() + 10);
	mDummyImage->setTransferFunctions3D(transferFunction);
}

void TestTransferFunctions::setNewCorrupt2DTranferFunction()
{
	ssc::ImageLUT2DPtr transferFunction(new ssc::ImageLUT2D(mDummyImageData));
	transferFunction->setWindow(mDummyImage->getMax() + 10);
	mDummyImage->setLookupTable2D(transferFunction);
}

} /* namespace cxtest */

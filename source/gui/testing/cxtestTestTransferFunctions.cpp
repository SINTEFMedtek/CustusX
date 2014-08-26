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

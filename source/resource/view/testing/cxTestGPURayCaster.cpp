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

#ifndef WIN32

#include "cxTestGPURayCaster.h"
#include "stdint.h"
#include <vtkActor.h>
#include <vtkImageData.h>
#include "cxImage.h"
#include "sscGPURayCastVolumePainter.h"
#include "cxTypeConversions.h"

void TestGPURayCaster::setUp()
{
	mRep = cx::GPURayCastVolumeRep::New("GPURayCastVolumeRep_test");
}

void TestGPURayCaster::testSetImages()
{
	vtkImageDataPtr data = vtkImageData::New();
	cx::ImagePtr image1(new cx::Image("i1", data));
	cx::ImagePtr image2(new cx::Image("i2", data));
	cx::ImagePtr image3(new cx::Image("i3", data));
	cx::ImagePtr image4(new cx::Image("i4", data));
	cx::ImagePtr image5(new cx::Image("i5", data));
	std::vector<cx::ImagePtr> images;
	uint64_t mTime = mRep->mActor->GetMTime();
	images.push_back(image1);
	CPPUNIT_ASSERT_EQUAL((size_t)0, mRep->getImages().size());
	mRep->setImages(images);
	CPPUNIT_ASSERT_EQUAL((size_t)1, mRep->getImages().size());
	CPPUNIT_ASSERT_EQUAL(QString("i1"), mRep->getImages()[0]->getUid());
	CPPUNIT_ASSERT(mRep->mActor->GetMTime() > mTime);
	mTime = mRep->mActor->GetMTime();
	mRep->setImages(images);
	uint64_t mNewTime = mRep->mActor->GetMTime();
	CPPUNIT_ASSERT_EQUAL(mTime, mNewTime);
	
	images.push_back(image2);
	images.push_back(image3);
	images.push_back(image4);
	images.push_back(image5);
	mRep->setImages(images);
	CPPUNIT_ASSERT_EQUAL((size_t)4, mRep->getImages().size());
	CPPUNIT_ASSERT_EQUAL(QString("i1"), mRep->getImages()[0]->getUid());
	CPPUNIT_ASSERT_EQUAL(QString("i2"), mRep->getImages()[1]->getUid());
	CPPUNIT_ASSERT_EQUAL(QString("i3"), mRep->getImages()[2]->getUid());
	CPPUNIT_ASSERT_EQUAL(QString("i4"), mRep->getImages()[3]->getUid());
}

void TestGPURayCaster::testParameters()
{
	uint64_t mTime = mRep->mActor->GetMTime();
	CPPUNIT_ASSERT(!mRep->mPainter->mShouldResample);
	CPPUNIT_ASSERT_EQUAL(0, mRep->mPainter->mDownsamplePixels);
	mRep->enableImagePlaneDownsampling(10000);
	CPPUNIT_ASSERT_EQUAL(10000, mRep->mPainter->mDownsamplePixels);
	CPPUNIT_ASSERT(mRep->mPainter->mShouldResample);
	CPPUNIT_ASSERT(mRep->mActor->GetMTime() > mTime);
	
	mTime = mRep->mActor->GetMTime();
	mRep->disableImagePlaneDownsampling();
	CPPUNIT_ASSERT(!mRep->mPainter->mShouldResample);
	CPPUNIT_ASSERT(mRep->mActor->GetMTime() > mTime);

	mTime = mRep->mActor->GetMTime();
	CPPUNIT_ASSERT_EQUAL((float)1.0, mRep->mPainter->mStepSize);
	mRep->setStepSize(2.0);
	CPPUNIT_ASSERT_EQUAL((float)2.0, mRep->mPainter->mStepSize);
	CPPUNIT_ASSERT(mRep->mActor->GetMTime() > mTime);
}

CPPUNIT_TEST_SUITE_REGISTRATION( TestGPURayCaster );

#endif //WIN32

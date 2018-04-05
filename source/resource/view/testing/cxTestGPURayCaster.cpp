/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

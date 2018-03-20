/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTestVideoSource.h"
#include <QTimer>
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include "cxData.h"

namespace cx
{

TestVideoSource::TestVideoSource(QString uid, QString name, int width, int height)
{
	mUid = uid;
	mName = name;
	mConnected = false;
	mStreaming = false;
	mValidData = true;
	mInitialized = false;
	mFrames = 0;
	mResolution = 1;
	mWidth = width;
	mHeight = height;
	mImageImport = vtkImageImportPtr::New();
	setResolution(mResolution);
	mImageTimer = new QTimer(this);
	connect(mImageTimer, SIGNAL(timeout()), this, SLOT(processBuffer()));
	mBuffer = (uint8_t*)malloc(width * height * 3);

	mImageImport->SetDataScalarTypeToUnsignedChar();
	mImageImport->SetNumberOfScalarComponents(3);
	mImageImport->SetWholeExtent(0, mWidth - 1, 0, mHeight - 1, 0, 0);
	mImageImport->SetDataExtentToWholeExtent();
}

TestVideoSource::~TestVideoSource()
{
	stop();
	free(mBuffer);
}

void TestVideoSource::setResolution(double resolution)
{
	mResolution = resolution;
	mImageImport->SetDataSpacing(mResolution, mResolution, 1);
}

vtkImageDataPtr TestVideoSource::getVtkImageData()
{
	return mImageImport->GetOutput();
}

void TestVideoSource::start()
{
	if (mStreaming)
	{
		return;
	}
	if (!isConnected())
	{
		setConnected(true);
	}
	mStreaming = true;
	mImageTimer->start(40);
}

void TestVideoSource::stop()
{
	mImageTimer->stop();
	mStreaming = false;
}

static void TestImage(int width, int height, int frames, uint8_t *image, double mmPerPixel)
{
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			uint8_t *pix = &image[(y*width + x) * 3];
			pix[0] = (x*255/width)+frames;
			pix[1] = (y*255/height)+frames;
			pix[2] = 127;
			double mmPerPixel = 0.1;
			if ( (int)(x * mmPerPixel) % 10 == 0)
			{
				uint8_t *pix = &image[(y*width + x) * 3];
				pix[0] = 0;
				pix[1] = 0;
				pix[2] = 0;
			}
		}
	}
}

void TestVideoSource::processBuffer()
{
	TestImage(mWidth, mHeight, mFrames, mBuffer, mResolution);
	mFrames++;
	mImageImport->SetImportVoidPointer(mBuffer);
	mImageImport->Update();
	mImageImport->Modified();
	mInitialized = true;
	emit newFrame();
}

double TestVideoSource::getTimestamp()
{
	return (double) mImageImport->GetOutput()->GetMTime();
}

TimeInfo TestVideoSource::getAdvancedTimeInfo()
{
	TimeInfo retval;
	retval.mAcquisitionTime.setMSecsSinceEpoch(this->getTimestamp());
	return retval;
}

} // namespace cx

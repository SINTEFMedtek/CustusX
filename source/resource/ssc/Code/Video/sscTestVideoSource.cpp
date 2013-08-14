// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2012- SINTEF Medical Technology
// Copyright (C) 2012- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscTestVideoSource.h"
#include <QTimer>
#include <vtkImageImport.h>
#include <vtkImageData.h>

ssc::TestVideoSource::TestVideoSource(QString uid, QString name, int width, int height)
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

ssc::TestVideoSource::~TestVideoSource()
{
	stop();
	free(mBuffer);
}

void ssc::TestVideoSource::setResolution(double resolution)
{
	mResolution = resolution;
	mImageImport->SetDataSpacing(mResolution, mResolution, 1);
}

vtkImageDataPtr ssc::TestVideoSource::getVtkImageData()
{
	return mImageImport->GetOutput();
}

void ssc::TestVideoSource::start()
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

void ssc::TestVideoSource::stop()
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

void ssc::TestVideoSource::processBuffer()
{
	TestImage(mWidth, mHeight, mFrames, mBuffer, mResolution);
	mFrames++;
	mImageImport->SetImportVoidPointer(mBuffer);
	mImageImport->Update();
	mImageImport->Modified();
	mInitialized = true;
	emit newFrame();
}

double ssc::TestVideoSource::getTimestamp()
{
	return (double) mImageImport->GetOutput()->GetMTime();
}


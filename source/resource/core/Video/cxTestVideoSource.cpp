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

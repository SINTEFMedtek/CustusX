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

#include <vtkImageImport.h>
#include <vtkImageData.h>
#include "sscImageImportVideoSource.h"
#include "sscBoundingBox3D.h"

namespace ssc
{


ImageImportVideoSource::ImageImportVideoSource(QString uid, QString name)
{
	mUid = uid;
	mName = name;
	if (mName.isEmpty())
		mName = mUid;
	mConnected = false;
	mStreaming = false;
	mValidData = true;
	mResolution = 1;
	mTimestamp = 0;
	mImageImport = vtkImageImportPtr::New();
	this->setEmptyImage();
}

ImageImportVideoSource::~ImageImportVideoSource()
{
	stop();
}

void ImageImportVideoSource::setResolution(double resolution)
{
	mResolution = resolution;
	mImageImport->SetDataSpacing(mResolution, mResolution, 1);
}

vtkImageDataPtr ImageImportVideoSource::getVtkImageData()
{
	return mImageImport->GetOutput();
}

void ImageImportVideoSource::start()
{
	if (mStreaming)
		return;

	if (!isConnected())
		setConnected(true);

	mStreaming = true;
}

void ImageImportVideoSource::stop()
{
	if (!mStreaming)
		return;

	mStreaming = false;

	this->setEmptyImage();
	emit newFrame();
}

double ImageImportVideoSource::getTimestamp()
{
	return mTimestamp;
}

/**Return the vtkImageImport source that
 * is the source to the video.
 *
 */
vtkImageImportPtr ImageImportVideoSource::getImageImport()
{
	return mImageImport;
}

/**inform class that the imageimport filter has been
 * changed externally with the given timestamp.
 *
 */
void ImageImportVideoSource::refresh(double time)
{
	// return here causes text fields to be not-updated
//	if (!mStreaming)
//		return;
	mTimestamp = time;
	mImageImport->Update();
	mImageImport->Modified();

	emit newFrame();
}

/** crash-avoiding measure -  for startup
 */
void ImageImportVideoSource::setEmptyImage()
{
	mImageImport->SetWholeExtent(0, 1, 0, 1, 0, 0);
	mImageImport->SetDataExtentToWholeExtent();
	mImageImport->SetDataScalarTypeToUnsignedChar();
	mImageImport->SetNumberOfScalarComponents(1);
	std::fill(mZero.begin(), mZero.end(), 0);
	mImageImport->SetImportVoidPointer(mZero.begin());
//	mImageImport->GetOutput()->ComputeBounds();
//	mImageImport->GetOutput()->UpdateInformation();
//	mImageImport->GetOutput()->UpdateData();
	mImageImport->GetOutput()->Update();
	mImageImport->Modified();
}


} /* namespace ssc */

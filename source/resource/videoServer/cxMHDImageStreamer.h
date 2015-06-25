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

#ifndef CXMHDIMAGESTREAMER_H_
#define CXMHDIMAGESTREAMER_H_

#include "cxGrabberExport.h"

#include "boost/shared_ptr.hpp"
#include "cxStreamer.h"
#include "cxForwardDeclarations.h"

class QTimer;
class QDomElement;

namespace cx
{

typedef boost::shared_ptr<struct Package> PackagePtr;
typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;

vtkImageDataPtr loadImage(QString filename);
vtkLookupTablePtr createLookupTable(int numberOfTableValues);
vtkImageDataPtr applyLUTToImage(vtkImageDataPtr image, vtkLookupTablePtr lut);
vtkImageDataPtr convertToTestColorImage(vtkImageDataPtr image);

/**
 * Create test data for MHDImageStreamer
 *
 * \ingroup cx_resource_videoserver
 * \author Janne Beate Bakeng, SINTEF
 * \date May 16, 2013
 */
class cxGrabber_EXPORT ImageTestData
{
public:
	static ImageTestData initializePrimaryData(vtkImageDataPtr source, QString filename);
	static ImageTestData initializeSecondaryData(vtkImageDataPtr source, QString filename);
	static PackagePtr createPackage(ImageTestData* data);

	vtkImageDataPtr mImageData;
	boost::shared_ptr<class SplitFramesContainer> mDataSource;
	int mCurrentFrame;
	QString mRawUid;
};
typedef boost::shared_ptr<class ImageTestData> ImageTestDataPtr;

/**
 */
class cxGrabber_EXPORT ImageStreamerDummyArguments
{
public:
	QStringList getArgumentDescription();
	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	StringMap convertToCommandLineArguments(QDomElement root);

	FilePathPropertyPtr getFilenameOption(QDomElement root);
	BoolPropertyBasePtr getSecondaryOption(QDomElement root);
};


/**
 * Reads a mhd file from disk and sends slices at a given interval.
 *
 * \ingroup cx_resource_videoserver
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 * \date Jun 21, 2011
 */
class cxGrabber_EXPORT DummyImageStreamer: public CommandLineStreamer
{
Q_OBJECT

public:
	DummyImageStreamer();
	virtual ~DummyImageStreamer(){};

	virtual void initialize(QString filename, bool secondaryStream, bool sendonce = false);
	virtual void initialize(StringMap arguments);
	virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
	virtual bool isStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

private slots:
	virtual void streamSlot();

private:
	vtkImageDataPtr internalLoadImage(QString filename);
	QString getFileName();
	void setSendOnce(bool sendonce);
	bool mSendOnce;
	QString mFilename;

	bool shouldSetupSecondaryDataSource();
	vtkSmartPointer<vtkImageData> hasSecondaryData();
	void createTestDataSource(vtkImageDataPtr source);
	void sendTestDataFrames();

	bool mUseSecondaryStream;
	ImageTestData mPrimaryDataSource;
	ImageTestData mSecondaryDataSource;
};
typedef boost::shared_ptr<class DummyImageStreamer> DummyImageStreamerPtr;

}

#endif /* CXMHDIMAGESTREAMER_H_ */

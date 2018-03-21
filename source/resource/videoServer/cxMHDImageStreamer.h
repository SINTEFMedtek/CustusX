/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

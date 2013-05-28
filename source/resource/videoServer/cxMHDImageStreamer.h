#ifndef CXMHDIMAGESTREAMER_H_
#define CXMHDIMAGESTREAMER_H_

#include "boost/shared_ptr.hpp"
#include "cxImageStreamer.h"

class QTimer;

namespace cx
{

vtkImageDataPtr loadImage(QString filename);
vtkLookupTablePtr createLookupTable(int numberOfTableValues);
vtkImageDataPtr applyLUTToImage(vtkImageDataPtr image, vtkLookupTablePtr lut);
vtkImageDataPtr convertToTestColorImage(vtkImageDataPtr image);

/**
 * Create test data for MHDImageStreamer
 *
 * \author Janne Beate Bakeng, SINTEF
 * \date May 16, 2013
 */
class ImageTestData
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
 * Reads a mhd file from disk and sends slices at a given interval.
 *
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 * \date Jun 21, 2011
 */
class DummyImageStreamer: public ImageStreamer
{
Q_OBJECT

public:
	DummyImageStreamer();
	virtual ~DummyImageStreamer(){};

	virtual void initialize(QString filename, bool secondaryStream, bool sendonce = false);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();

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

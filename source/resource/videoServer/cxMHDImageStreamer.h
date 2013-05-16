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
class MHDImageStreamer: public ImageStreamer
{
Q_OBJECT

public:
	MHDImageStreamer();
	virtual ~MHDImageStreamer(){};

	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

private slots:
	void stream();

private:
	bool isInitialized();
	bool isReadyToSend();
	void createSendTimer();
	vtkImageDataPtr internalLoadImage(QString filename);
	QString getFileName();
	void setSendOnce();
	void setInitialized(bool initialized);
	bool mInitialized;
	bool mSendOnce;

	void initalizePrimaryAndSecondaryDataSource(vtkImageDataPtr source);
	unsigned long int shouldSetupSecondaryDataSource();
	vtkSmartPointer<vtkImageData> hasSecondaryData();
	void createTestDataSource(vtkImageDataPtr source);
	void sendTestDataFrames();
	ImageTestData mPrimaryDataSource;
	ImageTestData mSecondaryDataSource;

};
typedef boost::shared_ptr<class MHDImageStreamer> MHDImageStreamerPtr;

}

#endif /* CXMHDIMAGESTREAMER_H_ */

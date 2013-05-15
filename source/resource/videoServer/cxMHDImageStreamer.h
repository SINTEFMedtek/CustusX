#ifndef CXMHDIMAGESTREAMER_H_
#define CXMHDIMAGESTREAMER_H_

#include "boost/shared_ptr.hpp"
#include "cxImageStreamer.h"

class QTimer;

namespace cx
{

vtkImageDataPtr loadImage(QString filename);
vtkImageDataPtr convertToTestColorImage(vtkImageDataPtr input);

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * \author Christian Askeland, SINTEF
 * \date Jun 21, 2011
 */
class MHDImageStreamer: public ImageStreamer
{
Q_OBJECT

public:
	MHDImageStreamer();
	virtual ~MHDImageStreamer()
	{
	}

	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

private slots:
	void stream();

private:
	struct Data
	{
		vtkImageDataPtr mImageData;
		boost::shared_ptr<class SplitFramesContainer> mDataSource;
		int mCurrentFrame;
		QString mRawUid;
	};
	static Data initializePrimaryData(vtkImageDataPtr source, QString filename);
	static Data initializeSecondaryData(vtkImageDataPtr source, QString filename);
	PackagePtr createPackage(Data* data);
	bool isInitialized();
	bool isReadyToSend();
	void initalizePrimaryAndSecondaryData(vtkImageDataPtr source);

	bool mInitialized;
	bool mSendOnce;

	Data mPrimaryData;
	Data mSecondaryData;

};
typedef boost::shared_ptr<class MHDImageStreamer> MHDImageStreamerPtr;

}

#endif /* CXMHDIMAGESTREAMER_H_ */

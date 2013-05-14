#ifndef CXMHDIMAGESTREAMER_H_
#define CXMHDIMAGESTREAMER_H_

#include "boost/shared_ptr.hpp"
#include "cxImageStreamer.h"

class QTimer;

namespace cx
{

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
	virtual ~MHDImageStreamer() {}

	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

private slots:
	void tick();

private:
	struct Data
	{
		vtkImageDataPtr mImageData;
		boost::shared_ptr<class SplitFramesContainer> mDataSource;
		int mCurrentFrame;
		QString mRawUid; /// raw text to send as device name - excluding frame id
	};

	Data initializePrimaryData(vtkImageDataPtr source, QString filename) const;
	Data initializeSecondaryData(vtkImageDataPtr source, QString filename) const;
	void send(Data* data);
	void setTestImage();

	bool mSingleShot;

	Data mPrimaryData;
	Data mSecondaryData;

};
typedef boost::shared_ptr<class MHDImageStreamer> MHDImageStreamerPtr;
}

#endif /* CXMHDIMAGESTREAMER_H_ */

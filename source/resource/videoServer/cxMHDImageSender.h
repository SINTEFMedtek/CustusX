#ifndef CXMHDIMAGESENDER_H_
#define CXMHDIMAGESENDER_H_

#include "boost/shared_ptr.hpp"
#include "cxImageSender.h"

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
class MHDImageSender: public ImageStreamer
{

	Q_OBJECT

public:
	MHDImageSender(QObject* parent = NULL);
	virtual ~MHDImageSender() {}

	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(GrabberSenderPtr sender);
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

	//	GrabberSenderPtr mSender;
//	QTimer* mSendTimer;
//	StringMap mArguments;


	Data mPrimaryData;
	Data mSecondaryData;

};
typedef boost::shared_ptr<class MHDImageSender> MHDImageStreamerPtr;
}

#endif /* CXMHDIMAGESENDER_H_ */

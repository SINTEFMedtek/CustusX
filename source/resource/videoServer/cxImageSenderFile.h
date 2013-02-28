/*
 * cxImageSenderFile.h
 *
 *  \date Jun 21, 2011
 *      \author christiana
 */

#ifndef CXIMAGESENDERFILE_H_
#define CXIMAGESENDERFILE_H_

#include "boost/shared_ptr.hpp"
class QTimer;
#include "cxImageSender.h"


namespace cx
{

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 */
class MHDImageSender: public ImageSender
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

protected:
private:
	GrabberSenderPtr mSender;
	QTimer* mTimer;
	StringMap mArguments;

	struct Data
	{
		vtkImageDataPtr mImageData;
		boost::shared_ptr<class SplitFramesContainer> mDataSource;
		int mCurrentFrame;
		QString mRawUid; /// raw text to send as device name - excluding frame id
	};

	Data mPrimaryData;
	Data mSecondaryData;

	Data initializePrimaryData(vtkImageDataPtr source, QString filename) const;
	Data initializeSecondaryData(vtkImageDataPtr source, QString filename) const;
	void send(Data* data);
	void setTestImage();

private slots:
	void tick();
};

}

#endif /* CXIMAGESENDERFILE_H_ */

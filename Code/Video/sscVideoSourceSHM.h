/*
 * sscVideoSourceSHM.h
 *
 *  Created on: Oct 10, 2011
 *      Author: franka
 */

#ifndef SSCVIDEOSOURCESHMSTREAM_H_
#define SSCVIDEOSOURCESHMSTREAM_H_

#include <map>
#include <stdint.h>

#include <boost/shared_ptr.hpp>

#include "vtkSmartPointer.h"

#include <QObject>
#include <QDateTime>

#include "sscVideoSource.h"
#include "sscSharedMemory.h"

typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

// Forward declaration
class QTimer;

namespace ssc
{

/**
  * Contains data assosiated with a shared memory video stream
  */
class VideoSourceSHM : public VideoSource
{
	Q_OBJECT

public:

	VideoSourceSHM(int width = 0, int height = 0, int depth = 24);
	virtual ~VideoSourceSHM();

	int width() const { return mImageWidth; }
	int height() const { return mImageHeight; }
	int depth() const { return mImageColorDepth; }

	virtual QString getUid();
	virtual QString getName();
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();
	virtual void release();

	virtual QString getInfoString() const;
	virtual QString getStatusString() const;

	virtual void start();
	virtual void stop();

	virtual bool validData() const;
	virtual bool isConnected() const;
	virtual bool isStreaming() const;

	void connectServer(const QString& key);
	void disconnectServer();

protected:
	void update();

private:

	SharedMemoryClient	mSource;

	int			mImageWidth;
	int			mImageHeight;
	int			mImageColorDepth;

	vtkImageDataPtr		mImageData;
	vtkImageImportPtr	mImageImport;

	double			mTimeStamp;

	bool			mConnected;
	bool			mStreaming;
	bool			mEnablePolling;

	QTimer*			mPollTimer;

	void setEmptyImage();
	void setTestImage(int width, int height);

private slots:

	void serverPollSlot();
};

typedef boost::shared_ptr<VideoSourceSHM> VideoSourceSHMPtr;

} // namespace ssc

#endif /* SSCVIDEOSOURCESHMSTREAM_H_ */

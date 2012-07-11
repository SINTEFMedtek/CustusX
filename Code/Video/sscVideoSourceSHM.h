// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
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

/**\brief VideoSource for connecting to shared memory.
 *
 * Contains data assosiated with a shared memory video stream
 *
 * \ingroup sscVideo
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

	virtual QString getInfoString() const;
	virtual QString getStatusString() const;

	virtual void start();
	virtual void stop();

	virtual bool validData() const;
	virtual bool isConnected() const;
	virtual bool isStreaming() const;

	void connectServer(const QString& key);
	void disconnectServer();
	virtual void setResolution(double resolution);

protected:
	void update();

protected:
	int			mImageWidth;
	int			mImageHeight;
	int			mImageColorDepth;

private:

	SharedMemoryClient mSource;

	vtkImageDataPtr mImageData;
	vtkImageImportPtr mImageImport;

	double mTimeStamp;

	bool mConnected;
	bool mStreaming;
	bool mImportInitialized;
	bool mStartWhenConnected;

	QTimer* mPollTimer;

private slots:

	void serverPollSlot();
};

typedef boost::shared_ptr<VideoSourceSHM> VideoSourceSHMPtr;

} // namespace ssc

#endif /* SSCVIDEOSOURCESHMSTREAM_H_ */

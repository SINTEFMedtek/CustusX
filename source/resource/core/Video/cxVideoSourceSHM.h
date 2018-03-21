/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscVideoSourceSHM.h
 *
 *  Created on: Oct 10, 2011
 *      Author: franka
 */

#ifndef CXVIDEOSOURCESHM_H_
#define CXVIDEOSOURCESHM_H_

#include "cxResourceExport.h"

#include <map>
#include <stdint.h>

#include <boost/shared_ptr.hpp>

#include "vtkSmartPointer.h"

#include <QObject>
#include <QDateTime>

#include "cxVideoSource.h"
#include "cxSharedMemory.h"

typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

// Forward declaration
class QTimer;

namespace cx
{

/** \brief VideoSource for connecting to shared memory.
 *
 * Contains data assosiated with a shared memory video stream
 *
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT VideoSourceSHM : public VideoSource
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

} // namespace cx

#endif /* CXVIDEOSOURCESHM_H_ */

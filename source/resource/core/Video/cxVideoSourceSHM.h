/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


/*
 * sscVideoSourceSHM.h
 *
 *  Created on: Oct 10, 2011
 *      Author: franka
 */

#ifndef CXVIDEOSOURCESHM_H_
#define CXVIDEOSOURCESHM_H_

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

/**\brief VideoSource for connecting to shared memory.
 *
 * Contains data assosiated with a shared memory video stream
 *
 * \ingroup cx_resource_core_video
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

} // namespace cx

#endif /* CXVIDEOSOURCESHM_H_ */

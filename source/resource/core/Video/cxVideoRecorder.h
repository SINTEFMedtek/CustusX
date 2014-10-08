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
 * sscVideoRecorder.h
 *
 *  Created on: Dec 17, 2010
 *      Author: christiana
 */

#ifndef CXVIDEORECORDER_H_
#define CXVIDEORECORDER_H_

#include "cxResourceExport.h"

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "cxVideoSource.h"
#include <map>

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{

/**\brief Recorder for a VideoSource.
 *
 * The class has an autosync feature that is useful if the realtimestream clock
 * is unsynced with the master clock. The first received frame is used to estimate
 * a shift between the master and source clocks. This shift is then added to the input
 * times in getRecording in order to retrieve data using the source clock.
 *
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT VideoRecorder : public QObject
{
	Q_OBJECT
public:
	typedef std::map<double, vtkImageDataPtr> DataType; ///<  <timestamp, frame>
public:
	VideoRecorder(VideoSourcePtr source, bool sync = true);
	virtual ~VideoRecorder();

	virtual void startRecord();
	virtual void stopRecord();
	virtual DataType getRecording(double start, double stop);

	private slots:
	void newFrameSlot();
private:
	DataType mData;
	VideoSourcePtr mSource;

	bool mSynced;
	double mSyncShift;
};

typedef boost::shared_ptr<VideoRecorder> VideoRecorderPtr;

}

#endif /* CXVIDEORECORDER_H_ */

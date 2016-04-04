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
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef CXVIDEOSOURCE_H_
#define CXVIDEOSOURCE_H_

#include "cxResourceExport.h"

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "cxData.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{

/** \brief video source interface.
 *
 * Synchronize data with source,
 * provide data as a vtkImageData.
 *
 *
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT VideoSource : public QObject
{
	Q_OBJECT

public:
	virtual ~VideoSource() {}

	virtual QString getUid() = 0;
	virtual QString getName() = 0;
	virtual vtkImageDataPtr getVtkImageData() = 0;
	virtual double getTimestamp() = 0;
	virtual TimeInfo getAdvancedTimeInfo() = 0;

	virtual QString getInfoString() const = 0; ///< an information text intended to be visible along with the data stream at all times
	virtual QString getStatusString() const = 0; ///< status text describing the stream state, display instead of stream when the stream is invalid.

	virtual void start() = 0; ///< start streaming
	virtual void stop()  = 0; ///< stop streaming

	virtual bool validData() const = 0; ///< return true is data stream is ok to display. This is a heuristic based on the data rate.
	virtual bool isConnected() const = 0; ///< return true when a connection to the data source is established.
	virtual bool isStreaming() const = 0; ///< return true when the source is streaming data.

	/**
	 * Set the resolution of the incoming video in mm/pixel
	 */
	virtual void setResolution(double mmPerPixel) {}

signals:
	void streaming(bool on); ///< emitted when streaming started/stopped
	void connected(bool on); ///< emitted when source is connected/disconnected
	void newFrame();          ///< emitted when a new frame has arrived (getVtkImageData() returns something new). info/status/name/valid might also have changed
};
typedef boost::shared_ptr<VideoSource> VideoSourcePtr;

} // namespace cx

#endif /* CXVIDEOSOURCE_H_ */

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
#ifndef CXBASICVIDEOSOURCE_H
#define CXBASICVIDEOSOURCE_H

#include "cxResourceExport.h"

#include "vtkSmartPointer.h"
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include <boost/array.hpp>
class QTimer;

#include "cxVideoSource.h"
#include "cxForwardDeclarations.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageChangeInformation> vtkImageChangeInformationPtr;

namespace cx
{

/**
 * \brief VideoSource controlled by a vtkImageData
 *
 * Primitive implementation of the VideoSource interface.
 * Use the interface extensions to control the input:
 * Set a vtkImageData and
 *
 * \ingroup cx_resource_core_video
 * \date April 26, 2013
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT BasicVideoSource : public VideoSource
{
	Q_OBJECT

public:
	explicit BasicVideoSource(QString uid = "<none>");
	virtual ~BasicVideoSource();

	virtual QString getUid();
	virtual QString getName();
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();
	virtual TimeInfo getAdvancedTimeInfo();

	virtual QString getInfoString() const { return mInfo; }
	virtual QString getStatusString() const;

	virtual void start();
	virtual void stop();

	virtual bool validData() const;
	virtual bool isConnected() const;
	virtual bool isStreaming() const;

	virtual void setResolution(double resolution);

	// extensions:
	/** Set an image that is used for output.
	  *
	  * Set a new image for each new frame.
	  * Only the vtkImageData, uid and timestamp are
	  * used, also make sure the uid is unchanged for
	  * each setInput().
	  *
	  * Setting an input indicates connect(),
	  * settings an empty input indicates disconnect()
	  *
	  */
	void setInput(ImagePtr input);
	void setInfoString(QString text) { mInfo = text; }
	void setStatusString(QString text) { mStatus = text; }
	void deconfigure() { this->setInput(ImagePtr()); }
	/** Disable internal timeout checking,
	  * instead set timeout directly.
	  */
	void overrideTimeout(bool timeout);

private slots:
	void timeout();
private:
	bool mStreaming;
	QString mInfo;
	QString mStatus;

	ImagePtr mEmptyImage;
	ImagePtr mReceivedImage;
	vtkImageChangeInformationPtr mRedirecter;
	bool mTimeout;
	QTimer* mTimeoutTimer;
};

typedef boost::shared_ptr<BasicVideoSource> BasicVideoSourcePtr;

} // namespace cx

#endif // CXBASICVIDEOSOURCE_H

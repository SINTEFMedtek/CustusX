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

#ifndef CXIMAGESTREAMEROPENCV_H_
#define CXIMAGESTREAMEROPENCV_H_

#include "cxGrabberExport.h"

#include <QObject> //needed for the mocer when OpenCv is not used...
#include "cxVideoServerConfig.h"

#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QSize>
#include "igtlImageMessage.h"
#include <QStringList>
#include "cxIGTLinkImageMessage.h"
#include "cxSender.h"
#include "cxStreamer.h"

class QDomElement;


class QTimer;

namespace cv
{
	class VideoCapture;
}

namespace cx
{
typedef boost::shared_ptr<class Property> PropertyPtr;
typedef boost::shared_ptr<class DoublePropertyBase> DoublePropertyBasePtr;
typedef boost::shared_ptr<class BoolPropertyBase> BoolPropertyBasePtr;

typedef boost::shared_ptr<cv::VideoCapture> VideoCapturePtr;

/**
 */
class cxGrabber_EXPORT ImageStreamerOpenCVArguments
{
public:
	QStringList getArgumentDescription();
	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	StringMap convertToCommandLineArguments(QDomElement root);

	DoublePropertyBasePtr getVideoPortOption(QDomElement root);
	BoolPropertyBasePtr getPrintPropertiesOption(QDomElement root);
};

/**
 * An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * \ingroup cx_resource_videoserver
 * \author Christian Askeland, SINTEF
 * \date Jun 21, 2011
 *
 * This version uses openCV to grab images from a video file or camera
 */
class cxGrabber_EXPORT ImageStreamerOpenCV: public CommandLineStreamer
{
Q_OBJECT

public:
	ImageStreamerOpenCV();
	virtual ~ImageStreamerOpenCV();

	virtual void initialize(StringMap arguments);
	virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
	virtual bool isStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

private slots:
	void grab();
	void send();
	void continousGrabEvent();

private:
	void dumpProperties();
	IGTLinkImageMessage::Pointer getImageMessage();
	void dumpProperty(int val, QString name);

	void initialize_local();
	void deinitialize_local();

	QSize mRescaleSize;

	VideoCapturePtr mVideoCapture; // OpenCV video grabber
	QDateTime mLastGrabTime;
	bool mAvailableImage;
	bool mGrabbing;

};

}

#endif /* CXIMAGESTREAMEROPENCV_H_ */

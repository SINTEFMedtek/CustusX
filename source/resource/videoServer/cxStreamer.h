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

#ifndef CXSTREAMER_H_
#define CXSTREAMER_H_

#include "cxGrabberExport.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <map>
#include "boost/shared_ptr.hpp"
#include <iostream>

namespace cx
{

typedef boost::shared_ptr<class Sender> SenderPtr;
typedef boost::shared_ptr<class Streamer> StreamerPtr;

/**\brief
 *
 * \ingroup cx_resource_videoserver
 *
 * \date May 22, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGrabber_EXPORT Streamer : public QObject
{
	Q_OBJECT

public:
	Streamer();
	virtual ~Streamer(){};

	virtual void startStreaming(SenderPtr sender) = 0;
	virtual void stopStreaming() = 0;
	virtual bool isStreaming() = 0;

	void setSendInterval(int milliseconds); ///< how often an image should be sent (in milliseconds)
	int getSendInterval() const; ///< how often an image should be sent (in milliseconds)

signals:
	void stateChanged();

protected slots:
	virtual void streamSlot() = 0;

protected:
	void setInitialized(bool initialized);
	bool isInitialized();
	void createSendTimer(bool singleshot = false);
	bool isReadyToSend();
	SenderPtr mSender;
	QTimer* mSendTimer;

private:
	int mSendInterval; ///< how often an image should be sent (in milliseconds)
	bool mInitialized;

};

typedef std::map<QString, QString> StringMap;

/**\brief
 *
 *
 * \ingroup cx_resource_videoserver
 * \date May 22, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGrabber_EXPORT CommandLineStreamer : public Streamer
{
	Q_OBJECT

public:
	CommandLineStreamer(){};
	virtual ~CommandLineStreamer(){};

	virtual QStringList getArgumentDescription() = 0;
	virtual QString getType() = 0;

	virtual void initialize(StringMap arguments);

protected slots:
	virtual void streamSlot() {std::cout << "THIS SHOULD NOT HAPPEN...." << std::endl;};


protected:
	StringMap mArguments;
};
typedef boost::shared_ptr<CommandLineStreamer> CommandLineStreamerPtr;
}

#endif /* CXSTREAMER_H_ */

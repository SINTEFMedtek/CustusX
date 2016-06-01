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
#ifndef CXSTREAMERINTERFACE_H
#define CXSTREAMERINTERFACE_H

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QObject>
#include <QDomElement>
#include "cxProperty.h"
#include <vector>
class QWidget;

#define StreamerService_iid "cx::StreamerService"

namespace cx
{

// Use of smart pointer not possible for plugin service. Replace with serviceAdded and serviceRemoved signals using regular pointers
// The smart pointer can still be used if StreamerService is coming from another source than a ctk plugin
typedef boost::shared_ptr<class StreamerService> StreamerServicePtr;
typedef boost::shared_ptr<class Streamer> StreamerPtr;
typedef boost::shared_ptr<class Receiver> ReceiverPtr;

/**
 * \brief Abstract class. Interface to Streamers
 *
 * \ingroup cx_resource_core_video
 *
 * \date May 20, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT StreamerService : public QObject
{
	Q_OBJECT
public:
    static StreamerServicePtr getNullObject();

	StreamerService() {}
	virtual QString getName() = 0;
	virtual QString getType() const  = 0;
	/**
	 *  Fill settings for this algorithm.
	 *  Input is the root node for this algo, filled with stored settings (if any).
	 *  On completion, the root is filled with default values for settings.
	 */
	virtual std::vector<PropertyPtr> getSettings(QDomElement root) = 0;
	/**
	 * Return a streamer that can emit image data.
	 */
	virtual StreamerPtr createStreamer(QDomElement root) = 0;
};

/**
 * \brief Abstract class. Interface to Simulated Streamers
 *
 * \ingroup cx_resource_core_video
 *
 * \date Jun 01, 2016
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT SimulatedStreamerService : public StreamerService
{
	Q_OBJECT
public:
	SimulatedStreamerService() {}

public slots:
	virtual void setImageToStream(QString imageUid) = 0;
};

} //end namespace cx
Q_DECLARE_INTERFACE(cx::StreamerService, StreamerService_iid)

#endif // CXSTREAMERINTERFACE_H

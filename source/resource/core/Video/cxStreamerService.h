/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
//	virtual void testZZZ() {} //TODO: Check - uncommenting this cause seg fault??? Also moving stop() line (62) up here causes seg fault.
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
	virtual void stop() = 0;
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

//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXIMAGESTREAMERINTERFACE_H
#define CXIMAGESTREAMERINTERFACE_H

#include "boost/shared_ptr.hpp"
#include <QObject>
#include "cxVideoServiceBackend.h"
class QWidget;

#define StreamerService_iid "cx::StreamerService"

namespace cx
{

// Use of smart pointer not possible for plugin service. Replace with serviceAdded and serviceRemoved signals using regular pointers
// The smart pointer can still be used if StreamerService is coming from another source than a ctk plugin
typedef boost::shared_ptr<class StreamerService> StreamerServicePtr;
typedef boost::shared_ptr<class Streamer> StreamerPtr;

/**
 * \brief Abstract class. Interface to Streamers
 *
 * \ingroup cx_service_video
 *
 * \date May 20, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class StreamerService : public QObject
{
	Q_OBJECT
public:
	StreamerService() {}
	void setBackend(VideoServiceBackendPtr backend);
	virtual StreamerPtr createStreamer() = 0;
	virtual QWidget* createWidget() = 0;
	virtual QString getName() = 0;

protected:
	VideoServiceBackendPtr mBackend;
};

} //end namespace cx
Q_DECLARE_INTERFACE(cx::StreamerService, StreamerService_iid)

#endif // CXIMAGESTREAMERINTERFACE_H

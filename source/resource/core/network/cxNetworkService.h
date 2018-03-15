/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXNETWORKSERVICE_H
#define CXNETWORKSERVICE_H


#include "cxResourceExport.h"
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QStringList>
#include <QObject>

#define NetworkService_iid "cx::NetworkService"

namespace cx
{
typedef boost::shared_ptr<class NetworkServiceImpl> NetworkServiceImplPtr;
typedef boost::shared_ptr<class NetworkConnectionHandle> NetworkConnectionHandlePtr;

/**
 * Manages all network connections in CustusX.
 *
 *
 */
class cxResource_EXPORT NetworkService : public QObject
{
	Q_OBJECT
public:
	virtual ~NetworkService() {}
	virtual QStringList getConnectionUids() const = 0; // TODO use impl in this class
	virtual std::vector<NetworkConnectionHandlePtr> getConnections() const = 0;
	virtual NetworkConnectionHandlePtr getConnection(QString uid) = 0;
signals:
	void connectionsChanged();
};

} // namespace cx
Q_DECLARE_INTERFACE(cx::NetworkService, NetworkService_iid)


#endif // CXNETWORKSERVICE_H

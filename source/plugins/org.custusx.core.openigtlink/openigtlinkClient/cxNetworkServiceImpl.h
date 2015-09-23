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
#ifndef CXNETWORKSERVICEIMPL_H_
#define CXNETWORKSERVICEIMPL_H_

#include "boost/shared_ptr.hpp"
#include <QString>
#include <QObject>
#include "cxNetworkService.h"
#include "org_custusx_core_openigtlink_Export.h"

namespace cx
{
typedef boost::shared_ptr<class NetworkServiceImpl> NetworkConnectionManagerPtr;
typedef boost::shared_ptr<class NetworkConnectionHandle> NetworkConnectionHandlePtr;

/**
 * Manages all network connections in CustusX.
 *
 *
 */
class org_custusx_core_openigtlink_EXPORT NetworkServiceImpl : public NetworkService
{
	Q_OBJECT
public:
	NetworkServiceImpl();

	QStringList getConnectionUids() const;


	std::vector<NetworkConnectionHandlePtr> getConnections() const;
	NetworkConnectionHandlePtr getConnection(QString uid);
signals:
	void connectionsChanged();

private:
	QString newConnection(QString suggested_uid);
	QString findUniqueUidNumber(QString uidBase) const;
	NetworkConnectionHandlePtr findConnection(QString uid) const;
	std::vector<NetworkConnectionHandlePtr> mConnections;
};

} // namespace cx


#endif // CXNETWORKSERVICEIMPL_H_

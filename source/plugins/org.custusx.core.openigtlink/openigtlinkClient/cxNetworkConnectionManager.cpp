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
#include "cxNetworkConnectionManager.h"

#include "cxOpenIGTLinkClient.h"

namespace cx
{

NetworkConnectionManager::NetworkConnectionManager()
{
}

QString NetworkConnectionManager::newConnection(QString suggested_uid)
{
	QString uid = this->findUniqueUidNumber(suggested_uid);
	OpenIGTLinkClientThreadHandlerPtr connection(new OpenIGTLinkClientThreadHandler(uid));
	mConnections.push_back(connection);
	emit connectionsChanged();
	return uid;
}

QString NetworkConnectionManager::findUniqueUidNumber(QString uidBase) const
{
	int counter = 0;
	QString uid = uidBase;
	while (this->findConnection(uid))
	{
		counter++;
		uid = QString("%1%2").arg(uidBase).arg(counter);
	}

	return uid;
}

std::vector<OpenIGTLinkClientThreadHandlerPtr> NetworkConnectionManager::getConnections() const
{
	return mConnections;
}

QStringList NetworkConnectionManager::getConnectionUids() const
{
	QStringList retval;
	for (unsigned i=0; i<mConnections.size(); ++i)
		retval << mConnections[i]->client()->getUid();
	return retval;
}

OpenIGTLinkClientThreadHandlerPtr NetworkConnectionManager::findConnection(QString uid) const
{
	for (unsigned i=0; i<mConnections.size(); ++i)
		if (mConnections[i]->client()->getUid() == uid)
			return mConnections[i];
	return OpenIGTLinkClientThreadHandlerPtr();
}

OpenIGTLinkClientThreadHandlerPtr NetworkConnectionManager::getConnection(QString uid)
{
	OpenIGTLinkClientThreadHandlerPtr connection = this->findConnection(uid);

	if (!connection)
	{
		this->newConnection(uid);
		connection = this->findConnection(uid);
	}

	return connection;
}


} // namespace cx

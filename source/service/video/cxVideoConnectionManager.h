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

#ifndef CXVIDEOCONNECTION_H_
#define CXVIDEOCONNECTION_H_

#include "cxVideoServiceExport.h"

#include <vector>
#include <QObject>
#include <QProcess>
#include "cxForwardDeclarations.h"
#include "cxStreamerService.h"
#include "cxServiceTrackerListener.h"

namespace cx
{

typedef boost::shared_ptr<class ProcessWrapper> ProcessWrapperPtr;
typedef boost::shared_ptr<class VideoConnection> VideoConnectionPtr;
typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;

/**
* \file
* \addtogroup cx_service_video
* @{
*/

/**\brief
 *
 * \ingroup cx_service_video
 *
 *  \date Jan 25, 2011
 *  \author Christian Askeland, SINTEF
 */
class cxVideoService_EXPORT VideoConnectionManager: public QObject
{
Q_OBJECT
public:
	explicit VideoConnectionManager(VideoServiceBackendPtr backend);
	virtual ~VideoConnectionManager();

	QString getConnectionMethod();
	void setConnectionMethod(QString connectionMethod);

	void setLocalServerExecutable(QString commandline);
	QString getLocalServerExecutable();
	void setPort(int port);
	int getPort();
	QStringList getHostHistory();
	QString getHost();
	void setHost(QString host);
	QStringList getDirectLinkArgumentHistory();
	void setLocalServerArguments(QString commandline);
	QString getLocalServerArguments();

	bool useLocalServer();
	bool useDirectLink();
	bool useRemoteServer();
//	bool useSimulatedServer();

	void setInitScript(QString filename);
	QString getInitScript();

	void launchServer();
	void launchAndConnectServer(QString connectionMethod);
	void launchAndConnectServer();
	void disconnectServer();

	QProcess* getLocalVideoServerProcess();
	std::vector<VideoSourcePtr> getVideoSources();
	VideoConnectionPtr getVideoConnection();

	void setReconnectInterval(int interval);
	bool isConnected() const;

signals:
	void fps(QString source, int fps);
	void connected(bool on);
	void videoSourcesChanged();
	void connectionMethodChanged();

public slots:
	void connectServer();
	void serverProcessStateChanged(QProcess::ProcessState newState);

private:
	void onServiceAdded(StreamerService* service);
	void onServiceRemoved(StreamerService *service);
	void delayedAutoConnectServer();
	void runScript();
	bool localVideoServerIsRunning();
	void setupAndRunDirectLinkClient();
	void launchAndConnectUsingLocalServer();
	bool connectToService();

	QString mConnectionMethod;
	VideoConnectionPtr mVideoConnection;
	int mConnectWhenLocalServerRunning;
	int mReconnectInterval;
	ProcessWrapperPtr mLocalVideoServerProcess;
	ProcessWrapperPtr mIniScriptProcess;

	VideoServiceBackendPtr mBackend;

	boost::shared_ptr<ServiceTrackerListener<StreamerService> > mServiceListener;

};
typedef boost::shared_ptr<VideoConnectionManager> VideoConnectionManagerPtr;

/**
* @}
*/
}//end namespace cx

#endif /* CXVIDEOCONNECTION_H_ */

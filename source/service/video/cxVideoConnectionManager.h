// This file is part of CustusX, an Image Guided Therapy Application.
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

#ifndef CXVIDEOCONNECTION_H_
#define CXVIDEOCONNECTION_H_

#include <vector>
#include <QObject>
#include <QProcess>
#include "cxForwardDeclarations.h"
#include "sscXmlOptionItem.h"

namespace cx
{


typedef boost::shared_ptr<class ProcessWrapper> ProcessWrapperPtr;
typedef boost::shared_ptr<class VideoConnection> VideoConnectionPtr;

/**
* \file
* \addtogroup cxServiceVideo
* @{
*/

/**\brief Class representing a single IGTLink connection,
 * connection facilities and options.
 * \ingroup cxServiceVideo
 *
 * The connection can either operate on a remote server or a local one.
 * GUI can be found in cxIGTLinkWidget (along with some additional functionality...)
 *
 *  \date Jan 25, 2011
 *  \author christiana
 */
class VideoConnectionManager: public QObject
{
Q_OBJECT
public:
	VideoConnectionManager();
	virtual ~VideoConnectionManager();

	ssc::StringDataAdapterXmlPtr getConnectionMethod() { return mConnectionMethod; }

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

	void setInitScript(QString filename);
	QString getInitScript();

	void launchServer();
	void launchAndConnectServer();
	void disconnectServer();

	QProcess* getProcess();
	std::vector<ssc::VideoSourcePtr> getVideoSources();

	void setReconnectInterval(int interval) { mReconnectInterval = interval; }
	bool isConnected() const;

signals:
	void fps(int fps);
	void connected(bool on);
	void videoSourcesChanged();
	void connectionMethodChanged();

public slots:
	void connectServer();
	void serverProcessStateChanged(QProcess::ProcessState newState);

private:
	void delayedAutoConnectServer();

	VideoConnectionPtr mVideoConnection;
	int mConnectWhenLocalServerRunning;
	int mReconnectInterval;
	ProcessWrapperPtr mLocalVideoServerProcess;
	ProcessWrapperPtr mIniScriptProcess;

	ssc::StringDataAdapterXmlPtr mConnectionMethod;
	ssc::XmlOptionFile mOptions;

};
typedef boost::shared_ptr<VideoConnectionManager> VideoConnectionManagerPtr;

/**
* @}
*/
}//end namespace cx

#endif /* CXVIDEOCONNECTION_H_ */

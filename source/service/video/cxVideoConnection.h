/*
 * cxRTSourceManager.h
 *
 *  Created on: Jan 25, 2011
 *      Author: christiana
 */
#ifndef CXVIDEOCONNECTION_H_
#define CXVIDEOCONNECTION_H_

#include <vector>
#include <QtGui>
#include "sscView.h"
#include "cxOpenIGTLinkRTSource.h"
#include "cxRenderTimer.h"

namespace cx
{

/**Class representing a single IGTLink connection,
 * connection facilities and options.
 *
 * The connection can either operate on a remote server or a local one.
 * GUI can be found in cxIGTLinkWidget (along with some additional functionality...)
 *
 */
class VideoConnection: public QObject
{
Q_OBJECT
public:
	VideoConnection();
	virtual ~VideoConnection();

	void setLocalServerCommandLine(QString commandline);
	QString getLocalServerCommandLine();
	void setPort(int port);
	int getPort();
	QStringList getHostHistory();
	QString getHost();
	void setHost(QString host);

	void setUseLocalServer(bool use);
	bool getUseLocalServer();

	void launchServer();

	void launchAndConnectServer();

	QProcess* getProcess()
	{
		return mServer;
	}
	OpenIGTLinkRTSourcePtr getVideoSource()
	{
		return mRTSource;
	}

signals:
	void fps(int fps);
	void connected(bool on);

public slots:
	void connectServer();

private slots:
	void serverProcessStateChanged(QProcess::ProcessState newState);
	void serverProcessError(QProcess::ProcessError error);

private:
	void delayedAutoConnectServer();

	double mSoundSpeedCompensationFactor;
	OpenIGTLinkRTSourcePtr mRTSource;
	QProcess* mServer;
	int mConnectWhenLocalServerRunning;
};
typedef boost::shared_ptr<VideoConnection> VideoConnectionPtr;

}//end namespace cx

#endif /* CXVIDEOCONNECTION_H_ */

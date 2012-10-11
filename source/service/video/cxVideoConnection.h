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

/*
 * cxRTSourceManager.h
 *
 *  \date Jan 25, 2011
 *      \author christiana
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
	void setUseDirectLink(bool use);
	bool getUseDirectLink();

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
	void serverProcessReadyRead();

private slots:
	void serverProcessStateChanged(QProcess::ProcessState newState);
	void serverProcessError(QProcess::ProcessError error);

private:
	void delayedAutoConnectServer();

//	double mSoundSpeedCompensationFactor;
	OpenIGTLinkRTSourcePtr mRTSource;
	QProcess* mServer;
	int mConnectWhenLocalServerRunning;
};
typedef boost::shared_ptr<VideoConnection> VideoConnectionPtr;

/**
* @}
*/
}//end namespace cx

#endif /* CXVIDEOCONNECTION_H_ */

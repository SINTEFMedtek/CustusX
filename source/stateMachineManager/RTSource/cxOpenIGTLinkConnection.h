/*
 * cxOpenIGTLinkConnection.h
 *
 *  Created on: Jan 25, 2011
 *      Author: christiana
 */
#ifndef CXOPENIGTLINKCONNECTION_H_
#define CXOPENIGTLINKCONNECTION_H_

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"
#include "sscView.h"
#include "RTSource/sscOpenIGTLinkRTSource.h"
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
class IGTLinkConnection : public QObject
{
  Q_OBJECT
public:
  IGTLinkConnection();
  virtual ~IGTLinkConnection();

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

  QProcess* getProcess() { return mServer; }
  ssc::OpenIGTLinkRTSourcePtr getRTSource() { return mRTSource; }

public slots:
  void connectServer();

private slots:
  void serverProcessStateChanged(QProcess::ProcessState newState);
  void serverProcessError(QProcess::ProcessError error);
private:
  void delayedAutoConnectServer();

  ssc::OpenIGTLinkRTSourcePtr mRTSource;
  QProcess* mServer;
  int mConnectWhenLocalServerRunning;
};
typedef boost::shared_ptr<IGTLinkConnection> IGTLinkConnectionPtr;

}//end namespace cx

#endif /* CXOPENIGTLINKCONNECTION_H_ */

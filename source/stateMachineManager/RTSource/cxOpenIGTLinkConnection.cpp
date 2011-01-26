/*
 * cxOpenIGTLinkConnection.cpp
 *
 *  Created on: Jan 25, 2011
 *      Author: christiana
 */

#include "cxOpenIGTLinkConnection.h"
#include <QStringList>

#include "vtkRenderWindow.h"

#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscRTStreamRep.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"

namespace cx
{

IGTLinkConnection::IGTLinkConnection()
{
  mConnectWhenLocalServerRunning = 0;

  mServer = new QProcess(this);
  connect(mServer, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));
  connect(mServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));

  mRTSource.reset(new ssc::OpenIGTLinkRTSource());
  ssc::dataManager()->loadStream(mRTSource);
  //connect(mRTSource.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
}

IGTLinkConnection::~IGTLinkConnection()
{
  mRTSource->disconnectServer();
  // avoid getting crash reports: disable signal
  disconnect(mServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));
  mServer->close();
}

void IGTLinkConnection::setLocalServerCommandLine(QString commandline)
{
  DataLocations::getSettings()->setValue("IGTLink/localServer", commandline);
}

QString IGTLinkConnection::getLocalServerCommandLine()
{
  QString cmd = DataLocations::getSettings()->value("IGTLink/localServer").toString();
  if (cmd.isEmpty())
    cmd = "GrabberServer --auto";
  return cmd;
}

void IGTLinkConnection::setPort(int port)
{
  DataLocations::getSettings()->setValue("IGTLink/port", port);
}

int IGTLinkConnection::getPort()
{
  QVariant var = DataLocations::getSettings()->value("IGTLink/port");
  if (var.canConvert<int>())
    return var.toInt();
  return 18333;
}

void IGTLinkConnection::setUseLocalServer(bool use)
{
  DataLocations::getSettings()->setValue("IGTLink/useLocalServer", use);
}

bool IGTLinkConnection::getUseLocalServer()
{
  QVariant var = DataLocations::getSettings()->value("IGTLink/useLocalServer");
  if (var.canConvert<bool>())
    return var.toBool();
  return true;
}

/**Get list of recent hosts. The first is the current.
 *
 */
QStringList IGTLinkConnection::getHostHistory()
{
  QStringList hostHistory = DataLocations::getSettings()->value("IGTLink/hostHistory").toStringList();
  if (hostHistory.isEmpty())
    hostHistory << "Localhost";
  return hostHistory;
}

QString IGTLinkConnection::getHost()
{
  return this->getHostHistory().front(); // history will always contain elements.
}

void IGTLinkConnection::setHost(QString host)
{
  QStringList history = this->getHostHistory();
  history.prepend(host);
  for (int i=1; i<history.size(); ++i)
    if (history[i]==host)
      history.removeAt(i);
  while (history.size()>5)
    history.removeLast();

  DataLocations::getSettings()->setValue("IGTLink/hostHistory", history);
}


void IGTLinkConnection::launchServer()
{
  //  QString program = "/Users/christiana/christiana/workspace/CustusX3/build_RelWithDebInfo/modules/OpenIGTLinkServer/cxOpenIGTLinkServer";
  //  QStringList arguments;
  //  arguments << "18333" <<  "/Users/christiana/Patients/20101126T114627_Lab_66.cx3/US_Acq/USAcq_20100909T111205_5.mhd";


  QString commandline = this->getLocalServerCommandLine();

  if (commandline.isEmpty())
    return;
  if (mServer->state()!=QProcess::NotRunning)
    return;
  if (this->getHost().toUpper()!="LOCALHOST")
    return;

  QStringList text = commandline.split(" ");
  QString program = text[0];
  QStringList arguments = text;
  arguments.pop_front();

  ;
  if (!QFileInfo(program).isAbsolute())
    program = qApp->applicationDirPath() + "/" + program;

  if (!QFileInfo(program).exists())
  {
    ssc::messageManager()->sendError("Cannot find IGTLink server " + program);
    return;
  }

  ssc::messageManager()->sendInfo("Launching local IGTLink server " + program + " with arguments " + arguments.join(", "));

  if (mServer->state()==QProcess::NotRunning)
    mServer->start(program, arguments);
}

void IGTLinkConnection::connectServer()
{
  if (!mRTSource->isConnected())
  {
    if (this->getUseLocalServer())
      mRTSource->connectServer("LocalHost", this->getPort());
    else
      mRTSource->connectServer(this->getHost(), this->getPort());
  }

  this->delayedAutoConnectServer();
}

/** Attempt to connect to server at a later time if mConnectWhenLocalServerRunning >0,
 * and the server is unconnected.
 *
 */
void IGTLinkConnection::delayedAutoConnectServer()
{
  if (mRTSource->isConnected())
    mConnectWhenLocalServerRunning = 0;

  if (mConnectWhenLocalServerRunning)
  {
    --mConnectWhenLocalServerRunning;
    QTimer::singleShot(200, this, SLOT(connectServer())); // the process need some time to get its tcp server up and listening.
  }
}

void IGTLinkConnection::launchAndConnectServer()
{
  if (this->getUseLocalServer())
  {
    this->launchServer();

    if (mServer->state()!=QProcess::Running)
    {
      mConnectWhenLocalServerRunning = 3; // attempt 3 connects
    }
    else
    {
      this->connectServer();
    }
  }
  else
  {
    this->connectServer();
  }

}

void IGTLinkConnection::serverProcessError(QProcess::ProcessError error)
{
  QString msg;
  msg += "RT Source server reported an error: ";

  switch (error)
  {
  case QProcess::FailedToStart: msg += "Failed to start"; break;
  case QProcess::Crashed: msg += "Crashed"; break;
  case QProcess::Timedout: msg += "Timed out"; break;
  case QProcess::WriteError: msg += "Write Error"; break;
  case QProcess::ReadError: msg += "Read Error"; break;
  case QProcess::UnknownError: msg += "Unknown Error"; break;
  default: msg += "Invalid error";
  }

  ssc::messageManager()->sendError(msg);
}

void IGTLinkConnection::serverProcessStateChanged(QProcess::ProcessState newState)
{
  if (newState==QProcess::Running)
  {
    ssc::messageManager()->sendInfo("Local RT Source Server running.");
    this->delayedAutoConnectServer();
  }
  if (newState==QProcess::NotRunning)
  {
    ssc::messageManager()->sendInfo("Local RT Source Server not running.");
  }
  if (newState==QProcess::Starting)
  {
    ssc::messageManager()->sendInfo("Local RT Source Server starting.");
  }
}

}//end namespace cx

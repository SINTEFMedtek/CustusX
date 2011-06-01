/*
 * cxOpenIGTLinkConnection.cpp
 *
 *  Created on: Jan 25, 2011
 *      Author: christiana
 */

#include "cxVideoConnection.h"
#include <QStringList>

#include "vtkRenderWindow.h"

#include "sscLabeledComboBoxWidget.h"
#include "sscVideoRep.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "cxProbe.h"
#include "probeXmlConfigParser.h"

// must be removed!!!!
#include "sscToolManager.h"
#include "sscDataManager.h"

namespace cx
{

VideoConnection::VideoConnection()
{
  mConnectWhenLocalServerRunning = 0;

  mServer = new QProcess(this);
  connect(mServer, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));
  connect(mServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));

  mRTSource.reset(new OpenIGTLinkRTSource());
  ssc::dataManager()->loadStream(mRTSource);
  connect(getRTSource().get(), SIGNAL(connected(bool)), this, SLOT(connectSourceToTool()));
  connect(getRTSource().get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
  connect(mRTSource.get(), SIGNAL(fps(int)), this, SIGNAL(fps(int))); // thread-bridging connection

  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(connectSourceToTool()));
  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(connectSourceToTool()));
}

VideoConnection::~VideoConnection()
{
  mRTSource->disconnectServer();
  // avoid getting crash reports: disable signal
  disconnect(mServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));
  mServer->close();
}

void VideoConnection::setLocalServerCommandLine(QString commandline)
{
  settings()->setValue("IGTLink/localServer", commandline);
}

QString VideoConnection::getLocalServerCommandLine()
{
  QString cmd = settings()->value("IGTLink/localServer").toString();
  if (cmd.isEmpty())
    cmd = "GrabberServer.app --auto";
  return cmd;
}

void VideoConnection::setPort(int port)
{
  settings()->setValue("IGTLink/port", port);
}

int VideoConnection::getPort()
{
  QVariant var = settings()->value("IGTLink/port");
  if (var.canConvert<int>())
    return var.toInt();
  return 18333;
}

void VideoConnection::setUseLocalServer(bool use)
{
  settings()->setValue("IGTLink/useLocalServer", use);
}

bool VideoConnection::getUseLocalServer()
{
  QVariant var = settings()->value("IGTLink/useLocalServer");
  if (var.canConvert<bool>())
    return var.toBool();
  return true;
}

/**Get list of recent hosts. The first is the current.
 *
 */
QStringList VideoConnection::getHostHistory()
{
  QStringList hostHistory = settings()->value("IGTLink/hostHistory").toStringList();
  if (hostHistory.isEmpty())
    hostHistory << "Localhost";
  return hostHistory;
}

QString VideoConnection::getHost()
{
  return this->getHostHistory().front(); // history will always contain elements.
}

void VideoConnection::setHost(QString host)
{
  QStringList history = this->getHostHistory();
  history.prepend(host);
  for (int i=1; i<history.size(); ++i)
    if (history[i]==host)
      history.removeAt(i);
  while (history.size()>5)
    history.removeLast();

  settings()->setValue("IGTLink/hostHistory", history);
}


void VideoConnection::launchServer()
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

  if (!QFileInfo(program).isAbsolute())
    program = DataLocations::getBundlePath() + "/" + program;

  if (!QFileInfo(program).exists())
  {
    ssc::messageManager()->sendError("Cannot find IGTLink server " + program);
    return;
  }

  ssc::messageManager()->sendInfo("Launching local IGTLink server " + program + " with arguments " + arguments.join(", "));

  if (mServer->state()==QProcess::NotRunning)
    mServer->start(program, arguments);
}

void VideoConnection::connectServer()
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
void VideoConnection::delayedAutoConnectServer()
{
  if (mRTSource->isConnected())
    mConnectWhenLocalServerRunning = 0;

  if (mConnectWhenLocalServerRunning)
  {
    --mConnectWhenLocalServerRunning;
    QTimer::singleShot(400, this, SLOT(connectServer())); // the process need some time to get its tcp server up and listening. GrabberServer seems to need more than 500ms
  }
}

void VideoConnection::launchAndConnectServer()
{
  if (this->getUseLocalServer())
  {
    this->launchServer();

    if (mServer->state()!=QProcess::Running)
    {
      mConnectWhenLocalServerRunning = 5; // attempt N connects
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

void VideoConnection::serverProcessError(QProcess::ProcessError error)
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

void VideoConnection::serverProcessStateChanged(QProcess::ProcessState newState)
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

/**insert the rt source into the (first) probe tool
 * in the tool manager.
 *
 * Apply time calibration to the source.
 *
 */
void VideoConnection::connectSourceToTool()
{
  if (!mRTSource)
 {
    ssc::messageManager()->sendError("no rt source.");
    return;
 }

//    return;
  // find probe in tool manager
  // set source in cxTool
  // insert timecalibration using config
  if (!mRTSource->isConnected())
    return;

  if (mProbe)
    return;

  ssc::ToolPtr probe = this->findSuitableProbe();
  if (!probe)
    return;

  mProbe = probe;

  if (mProbe)
  {
    ProbePtr probeInterface = boost::shared_dynamic_cast<Probe>(mProbe->getProbe());
    if (!probeInterface)
    {
      ssc::messageManager()->sendError("Probe not a cx instance.");
      return;
    }
    probeInterface->setRTSource(mRTSource);
    ssc::toolManager()->setDominantTool(mProbe->getUid());
//    std::cout << "VideoConnection::connectSourceToTool() " << probe->getUid() << " " << probeInterface->getRTSource()->getName() << " completed" << std::endl;
  }
}

ssc::ToolPtr VideoConnection::getStreamingProbe()
{
  return mProbe;
}

/**Find a probe that can be connected to a rt source.
 *
 */
ssc::ToolPtr VideoConnection::findSuitableProbe()
{
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();

  // look for visible probes
  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    if (iter->second->getProbe() && iter->second->getProbe()->isValid() && iter->second->getVisible())
    {
      return iter->second;
    }
  }

  // pick the first probe, visible or not.
  for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    if (iter->second->getProbe() && iter->second->getProbe()->isValid())
    {
      return iter->second;
    }
  }

  return ssc::ToolPtr();
}


}//end namespace cx

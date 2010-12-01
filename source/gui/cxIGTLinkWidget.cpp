#include "cxIGTLinkWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "vtkRenderWindow.h"

#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
//#include "RTSource/cxIGTLinkClient.h"
#include "RTSource/sscRT2DRep.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"

namespace cx
{



IGTLinkWidget::IGTLinkWidget(QWidget* parent) :
    QWidget(parent)
{
  mServer = NULL;
  mView = NULL;
  mRenderTimer = NULL;

  this->setObjectName("IGTLinkWidget");
  this->setWindowTitle("IGTLink Test");
  this->resize(100, 600);

  mRTSource.reset(new ssc::OpenIGTLinkRTSource());
  ssc::dataManager()->loadStream(mRTSource);
  connect(mRTSource.get(), SIGNAL(serverStatusChanged()), this, SLOT(serverStatusChangedSlot()));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  mToptopLayout = toptopLayout;

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);
  mGridLayout = gridLayout;

  QStringList hostHistory = DataLocations::getSettings()->value("IGTLink/hostHistory").toStringList();
  if (hostHistory.isEmpty())
    hostHistory << "127.0.0.1";

  gridLayout->addWidget(new QLabel("IP Address", this), 0, 0);
  mAddressEdit = new QComboBox(this);
  mAddressEdit->setEditable(true);
  mAddressEdit->setInsertPolicy(QComboBox::InsertAtTop);
  mAddressEdit->addItems(hostHistory);

  gridLayout->addWidget(mAddressEdit, 0, 1);

  gridLayout->addWidget(new QLabel("Port number", this), 1, 0);
  mPortEdit = new QLineEdit(this);
  mPortEdit->setText("18333");
  gridLayout->addWidget(mPortEdit, 1, 1);

  mLaunchServerButton = new QPushButton("launch image server", this);
  connect(mLaunchServerButton, SIGNAL(clicked()), this, SLOT(launchServer()));
  gridLayout->addWidget(mLaunchServerButton, 2, 1);

  mShowStreamButton = new QPushButton("show stream", this);
  connect(mShowStreamButton, SIGNAL(clicked()), this, SLOT(showStream()));
  gridLayout->addWidget(mShowStreamButton, 3, 1);

  mConnectButton = new QPushButton("Connect Server", this);
  connect(mConnectButton, SIGNAL(clicked()), this, SLOT(toggleConnect()));
  gridLayout->addWidget(mConnectButton, 4, 1);

  toptopLayout->addStretch();
}

IGTLinkWidget::~IGTLinkWidget()
{
  QStringList hostHistory;
  for (int i=0; i<mAddressEdit->count(); ++i)
    hostHistory << mAddressEdit->itemText(i);
  DataLocations::getSettings()->setValue("IGTLink/hostHistory", hostHistory);
}

void IGTLinkWidget::updateHostHistory()
{
  mAddressEdit->blockSignals(true);

  if (mAddressEdit->currentIndex()!=0)
  {
    QString from = mAddressEdit->currentText();

    mAddressEdit->removeItem(mAddressEdit->currentIndex());
    mAddressEdit->insertItem(0, from);
    mAddressEdit->setCurrentIndex(0);
  }

  while (mAddressEdit->count() > 5)
  {
    mAddressEdit->removeItem(mAddressEdit->count()-1);
  }
  mAddressEdit->blockSignals(false);
}

void IGTLinkWidget::showEvent(QShowEvent* event)
{
  if (mRenderTimer)
    mRenderTimer->start();
}

void IGTLinkWidget::hideEvent(QHideEvent* event)
{
  if (mRenderTimer)
    mRenderTimer->stop();
}

void IGTLinkWidget::renderSlot()
{
  if (this->visibleRegion().isEmpty()) // needed to avoid drawing errors: checking visible is not enough
    return;

  mRenderTimerW.beginRender();

  if (mView->isVisible())
    mView->GetRenderWindow()->Render();

  mRenderTimerW.endRender();

  if (mRenderTimerW.intervalPassed())
  {
    mRenderLabel->setText(QString::number(mRenderTimerW.getFPS(),'f',0)+ "FPS");
    mRenderTimerW.reset();
  }
}

void IGTLinkWidget::launchServer()
{
  QString program = "/Users/christiana/christiana/workspace/CustusX3/build_RelWithDebInfo/modules/OpenIGTLinkServer/cxOpenIGTLinkServer";
  QStringList arguments;
  arguments << "18333" <<  "/Users/christiana/Patients/20101126T114627_Lab_66.cx3/US_Acq/USAcq_20100909T111205_5.mhd";

  if (!mServer)
  {
    mServer = new QProcess(this);
    connect(mServer, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));
    connect(mServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));
  }

  if (mServer->state()==QProcess::NotRunning)
    mServer->start(program, arguments);
  else if (mServer->state()==QProcess::Running)
    mServer->close();
}

void IGTLinkWidget::serverProcessError(QProcess::ProcessError error)
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

void IGTLinkWidget::serverProcessStateChanged(QProcess::ProcessState newState)
{
  if (newState==QProcess::Running)
  {
    ssc::messageManager()->sendInfo("Local RT Source Server running.");
    mLaunchServerButton->setText("Close Local Server");
  }
  if (newState==QProcess::NotRunning)
  {
    ssc::messageManager()->sendInfo("Local RT Source Server not running.");
    mLaunchServerButton->setText("Launch Local Server");
  }
  if (newState==QProcess::Starting)
  {
    ssc::messageManager()->sendInfo("Local RT Source Server starting.");
    mLaunchServerButton->setText("Starting...");
  }
}


void IGTLinkWidget::showStream()
{
  if (mView)
    return;

  mView = new ssc::View();
  mView->setBackgoundColor(QColor("khaki"));
  mToptopLayout->addWidget(mView, 1);
  mRenderTimer = new QTimer(this);
  std::cout << "start stream" << std::endl;
  connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(renderSlot()));
  mRenderTimer->setInterval(50);
  mRenderTimer->start();

  ssc::RealTimeStreamFixedPlaneRepPtr rtRep(new ssc::RealTimeStreamFixedPlaneRep("rtrep", "rtrep"));
  rtRep->setRealtimeStream(mRTSource);
//  rtRep->setTool(ssc::toolManager()->getDominantTool());
  mView->addRep(rtRep);

  mRenderLabel = new QLabel("-");
  mToptopLayout->addWidget(mRenderLabel);
}

void IGTLinkWidget::toggleConnect()
{
  if (!mRTSource->connected())
  {
    this->updateHostHistory();

    mRTSource->connectServer(mAddressEdit->currentText(), mPortEdit->text().toInt());
  }
  else
  {
    mRTSource->disconnectServer();
  }
}

void IGTLinkWidget::serverStatusChangedSlot()
{
  if (mRTSource->connected())
    mConnectButton->setText("Disconnect Server");
  else
    mConnectButton->setText("Connect Server");

  this->adjustSize();
//  mConnectButton->adjustSize();
//  mView->adjustSize();
//  mConnectButton->update();
//  mGridLayout->invalidate();
//  mToptopLayout->invalidate();
}



}//end namespace cx

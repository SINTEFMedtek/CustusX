#include "cxIGTLinkWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "vtkRenderWindow.h"

#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscVideoRep.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "cxVideoConnection.h"
//#include "cxStateService.h"
#include "cxVideoService.h"

namespace cx
{


IGTLinkWidget::IGTLinkWidget(QWidget* parent) :
    BaseWidget(parent, "IGTLinkWidget", "IGTLink Client")
{
  mView = NULL;
  mRenderTimer = NULL;

  this->resize(100, 600);

  connect(getRTSource().get(), SIGNAL(connected(bool)), this, SLOT(serverStatusChangedSlot()));
  connect(getServer(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  mToptopLayout = toptopLayout;

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);
  mGridLayout = gridLayout;


  gridLayout->addWidget(new QLabel("IP Address", this), 0, 0);
  mAddressEdit = new QComboBox(this);
  mAddressEdit->setEditable(true);
  mAddressEdit->setInsertPolicy(QComboBox::InsertAtTop);
  this->updateHostHistory();
  gridLayout->addWidget(mAddressEdit, 0, 1);

  gridLayout->addWidget(new QLabel("Port number", this), 1, 0);
  mPortEdit = new QLineEdit(this);
  mPortEdit->setText(QString::number(getConnection()->getPort()));
  gridLayout->addWidget(mPortEdit, 1, 1);

  QAction* browseLocalServerAction = new QAction(QIcon(":/icons/open.png"), "Browse", this);
  browseLocalServerAction->setStatusTip("Select a local server application");
  connect(browseLocalServerAction, SIGNAL(triggered()), this, SLOT(browseLocalServerSlot()));
  QToolButton* button = new QToolButton();
  button->setDefaultAction(browseLocalServerAction);
  gridLayout->addWidget(button, 3, 2);

  mUseLocalServer = new QCheckBox("Use Local Server", this);
  mUseLocalServer->setChecked(getConnection()->getUseLocalServer());
  connect(mUseLocalServer, SIGNAL(toggled(bool)), this, SLOT(useLocalServerChanged()));
  gridLayout->addWidget(mUseLocalServer, 2, 0, 1, 2);

  gridLayout->addWidget(new QLabel("Local Server", this), 3, 0);
  mLocalServerEdit = new QLineEdit(this);
  mLocalServerEdit->setText(getConnection()->getLocalServerCommandLine());
  gridLayout->addWidget(mLocalServerEdit, 3, 1);

  QHBoxLayout* buttonsLayout = new QHBoxLayout;
  toptopLayout->addLayout(buttonsLayout);

  mLaunchServerButton = new QPushButton("Launch Local Server", this);
  connect(mLaunchServerButton, SIGNAL(clicked()), this, SLOT(toggleLaunchServer()));
  buttonsLayout->addWidget(mLaunchServerButton);

  mShowStreamButton = new QPushButton("Show Stream View", this);
  connect(mShowStreamButton, SIGNAL(clicked()), this, SLOT(showStream()));
  buttonsLayout->addWidget(mShowStreamButton);

  mConnectButton = new QPushButton("Connect Server", this);
  connect(mConnectButton, SIGNAL(clicked()), this, SLOT(toggleConnectServer()));
  toptopLayout->addWidget(mConnectButton);

  toptopLayout->addStretch();

  this->dataChanged();
}

QString IGTLinkWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3><Setup IGTLink connection.</h3>"
      "<p>Lets you set up a connection to a streaming server using IGTLink.</p>"
      "<p><i></i></p>"
      "</html>";
}

QProcess* IGTLinkWidget::getServer()
{
  return getConnection()->getProcess();
}

OpenIGTLinkRTSourcePtr IGTLinkWidget::getRTSource()
{
  return getConnection()->getVideoSource();
}

VideoConnectionPtr IGTLinkWidget::getConnection()
{
  return videoService()->getIGTLinkVideoConnection();
}

IGTLinkWidget::~IGTLinkWidget()
{
}

void IGTLinkWidget::useLocalServerChanged()
{
	if (mUseLocalServer->isChecked())
	{
		mAddressEdit->addItem("Localhost");
		mAddressEdit->setCurrentIndex(mAddressEdit->count()-1);
	}

	this->writeSettings();
	this->dataChanged();
}

void IGTLinkWidget::dataChanged()
{
  mAddressEdit->setEnabled(!getConnection()->getUseLocalServer());
  mLocalServerEdit->setEnabled(getConnection()->getUseLocalServer());
  mLaunchServerButton->setEnabled(getConnection()->getUseLocalServer());

  mShowStreamButton->setEnabled(getConnection()->getVideoSource()->isConnected());
}

void IGTLinkWidget::updateHostHistory()
{
  mAddressEdit->blockSignals(true);
  mAddressEdit->clear();
  mAddressEdit->addItems(getConnection()->getHostHistory());
  mAddressEdit->blockSignals(false);
}

void IGTLinkWidget::browseLocalServerSlot()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select Server"), "~");
  if (fileName.isEmpty())
    return;
  mLocalServerEdit->setText(fileName);
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
  this->writeSettings();
  getConnection()->launchServer();
}

void IGTLinkWidget::toggleLaunchServer()
{
  if (( getServer() )&&( getServer()->state()==QProcess::Running ))
    getServer()->close();
  else
    this->launchServer();
}


void IGTLinkWidget::serverProcessStateChanged(QProcess::ProcessState newState)
{
  if (newState==QProcess::Running)
  {
    mLaunchServerButton->setText("Close Local Server");
  }
  if (newState==QProcess::NotRunning)
  {
    mLaunchServerButton->setText("Launch Local Server");
  }
  if (newState==QProcess::Starting)
  {
    mLaunchServerButton->setText("Starting...");
  }
}

void IGTLinkWidget::showStream()
{
  if (mView)
    return;

  mView = new ssc::ViewWidget();
  mView->setBackgroundColor(QColor("khaki"));
  mToptopLayout->addWidget(mView, 1);
  mRenderTimer = new QTimer(this);
  connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(renderSlot()));
  mRenderTimer->setInterval(50);
  mRenderTimer->start();

  ssc::VideoFixedPlaneRepPtr rtRep(new ssc::VideoFixedPlaneRep("rtrep", "rtrep"));
  rtRep->setRealtimeStream(getRTSource());
//  rtRep->setTool(ssc::toolManager()->getDominantTool());
  mView->addRep(rtRep);

  mRenderLabel = new QLabel("-");
  mToptopLayout->addWidget(mRenderLabel);
}

void IGTLinkWidget::toggleConnectServer()
{
  if (!getRTSource()->isConnected())
  {
    this->connectServer();
  }
  else
  {
    getRTSource()->disconnectServer();
  }
}

void IGTLinkWidget::writeSettings()
{
//  std::cout << "guiChanged" << std::endl;
  getConnection()->setLocalServerCommandLine(mLocalServerEdit->text());
  getConnection()->setHost(mAddressEdit->currentText());
  getConnection()->setPort(mPortEdit->text().toInt());
  getConnection()->setUseLocalServer(mUseLocalServer->isChecked());
  this->updateHostHistory();
}

void IGTLinkWidget::connectServer()
{
  if (!getRTSource()->isConnected())
  {
    this->writeSettings();
    getConnection()->launchAndConnectServer();
  }
}

void IGTLinkWidget::serverStatusChangedSlot()
{
  if (getRTSource()->isConnected())
    mConnectButton->setText("Disconnect Server");
  else
    mConnectButton->setText("Connect Server");

  this->adjustSize();
}

}//end namespace cx

#include "cxIGTLinkWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "vtkRenderWindow.h"

#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
//#include "RTSource/cxIGTLinkClient.h"
#include "RTSource/sscRT2DRep.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "sscToolManager.h"

namespace cx
{



IGTLinkWidget::IGTLinkWidget(QWidget* parent) :
    QWidget(parent)
{
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

  gridLayout->addWidget(new QLabel("IP Address", this), 0, 0);
  mAddressEdit = new QLineEdit(this);
  mAddressEdit->setText("127.0.0.1");
  gridLayout->addWidget(mAddressEdit, 0, 1);

  gridLayout->addWidget(new QLabel("Port number", this), 1, 0);
  mPortEdit = new QLineEdit(this);
  mPortEdit->setText("18333");
  gridLayout->addWidget(mPortEdit, 1, 1);

//  mLaunchServerButton = new QPushButton("launch image server", this);
//  connect(mLaunchServerButton, SIGNAL(clicked()), this, SLOT(launchServer()));
//  gridLayout->addWidget(mLaunchServerButton, 2, 1);

  mShowStreamButton = new QPushButton("show stream", this);
  connect(mShowStreamButton, SIGNAL(clicked()), this, SLOT(showStream()));
  gridLayout->addWidget(mShowStreamButton, 2, 1);

  mConnectButton = new QPushButton("Connect Server", this);
  connect(mConnectButton, SIGNAL(clicked()), this, SLOT(toggleConnect()));
  gridLayout->addWidget(mConnectButton, 3, 1);

  toptopLayout->addStretch();
}

IGTLinkWidget::~IGTLinkWidget()
{
}

void IGTLinkWidget::showEvent(QShowEvent* event)
{
  if (mRenderTimer)
    mRenderTimer->start();
  std::cout << "render start" << std::endl;

}

void IGTLinkWidget::hideEvent(QHideEvent* event)
{
  if (mRenderTimer)
    mRenderTimer->stop();
  std::cout << "render stop" << std::endl;
}

void IGTLinkWidget::renderSlot()
{
//  std::cout << "arender" << std::endl;
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
//  std::cout << "render" << std::endl;
}

void IGTLinkWidget::launchServer()
{

}

void IGTLinkWidget::showStream()
{
  if (mView)
    return;

  mView = new ssc::View();
  mView->setBackgoundColor(QColor("khaki"));
  mToptopLayout->addWidget(mView, 1);
  mRenderTimer = new QTimer(this);
  connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(renderSlot()));
  mRenderTimer->setInterval(50);

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
    mRTSource->connectServer(mAddressEdit->text(), mPortEdit->text().toInt());
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

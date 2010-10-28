#include "cxIGTLinkWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxIGTLinkClient.h"

namespace cx
{



IGTLinkWidget::IGTLinkWidget(QWidget* parent) :
    QWidget(parent)
{
  this->setObjectName("IGTLinkWidget");
  this->setWindowTitle("IGTLink Test");

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);

  gridLayout->addWidget(new QLabel("IP Address", this), 0, 0);
  mAddressEdit = new QLineEdit(this);
  mAddressEdit->setText("127.0.0.1");
  gridLayout->addWidget(mAddressEdit, 0, 1);

  gridLayout->addWidget(new QLabel("Port number", this), 1, 0);
  mPortEdit = new QLineEdit(this);
  mPortEdit->setText("18944");
  gridLayout->addWidget(mPortEdit, 1, 1);

  mLaunchServerButton = new QPushButton("launch image server", this);
  connect(mLaunchServerButton, SIGNAL(clicked()), this, SLOT(launchServer()));
  gridLayout->addWidget(mLaunchServerButton, 2, 1);

  mConnectButton = new QPushButton("connect to image server", this);
  connect(mConnectButton, SIGNAL(clicked()), this, SLOT(toggleConnect()));
  gridLayout->addWidget(mConnectButton, 3, 1);

  toptopLayout->addStretch();
}

IGTLinkWidget::~IGTLinkWidget()
{
  if (mClient)
  {
    mClient->terminate();
    mClient->wait(); // forever or until dead thread
  }
}

void IGTLinkWidget::launchServer()
{

}

void IGTLinkWidget::toggleConnect()
{
  std::cout << "toggleconnect " << mClient  << std::endl;

  if (mClient)
  {
    this->disconnectServer();
  }
  else
  {
    this->connectServer();
  }
}

void IGTLinkWidget::connectServer()
{
  if (mClient)
    return;
  std::cout << "connect to server" << std::endl;
  mClient.reset(new IGTLinkClient(mAddressEdit->text(), mPortEdit->text().toInt(), this));
  connect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
  mClient->start();

  mConnectButton->setText("Disconnect Server");
}

void IGTLinkWidget::disconnectServer()
{
  std::cout << "disconnect server" << std::endl;
  if (mClient)
  {
    mClient->stop();
    mClient->quit();
    mClient->wait(); // forever or until dead thread

    disconnect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
    mClient.reset();
  }

  mConnectButton->setText("Connect Server");
}

void IGTLinkWidget::clientFinishedSlot()
{
  if (!mClient)
    return;
  if (mClient->isRunning())
    return;
  this->disconnectServer();
}

void IGTLinkWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void IGTLinkWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}


}//end namespace cx

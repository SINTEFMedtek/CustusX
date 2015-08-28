#include "cxUr5Initialize.h"
#include "cxLogger.h"


#include <QGridLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QApplication>
#include <QThread>

namespace cx
{


Ur5InitializeTab::Ur5InitializeTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connect(connectButton,SIGNAL(clicked()),this,SLOT(connectButtonSlot()));
    connect(initializeButton,SIGNAL(clicked()),this,SLOT(initializeButtonSlot()));
    connect(disconnectButton,SIGNAL(clicked()),this,SLOT(disconnectButtonSlot()));   
    connect(shutdownButton,SIGNAL(clicked()),this,SLOT(shutdownButtonSlot()));
}

Ur5InitializeTab::~Ur5InitializeTab()
{
}

void Ur5InitializeTab::setupUi(QWidget *parent)
{   
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    int row = 0;
    ipLineEdit = new QLineEdit();
    connectButton = new QPushButton();
    mainLayout->addWidget(new QLabel("IP Address: "), row, 0, 1, 1);
    mainLayout->addWidget(ipLineEdit, row, 1,1,2);
    mainLayout->addWidget(connectButton,row,3,1,1);

    ipLineEdit->setText("169.254.62.100");

    QIcon icon;
    icon.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-idle.ico"), QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-transmit-receive.ico"), QSize(), QIcon::Normal, QIcon::On);
    connectButton->setIcon(icon);
    connectButton->setToolTip("Connect to robot");
    connectButton->setText("Connect");   
    connectButton->setCheckable(true);
    connectButton->setStyleSheet("QPushButton:checked { background-color: none; }");

    row ++;
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout -> addWidget(line,row,0,1,4);

    row ++;
    mainLayout->addWidget(new QLabel("Set origo for robot workspace"),row,0,1,2);

    row++;
    QTabWidget *setCoordinatesTab = new QTabWidget();
    QWidget *presetCoordinatesTab = new QWidget();
    QWidget *manualCoordinatesTab = new QWidget();
    setCoordinatesTab->addTab(presetCoordinatesTab,tr("Preset coordinates"));
    setCoordinatesTab->addTab(manualCoordinatesTab,tr("Manual coordinates"));
    mainLayout->addWidget(setCoordinatesTab,row,0,1,4);

    QHBoxLayout *presetCoordLayout = new QHBoxLayout(presetCoordinatesTab);
    presetCoordLayout->setAlignment(Qt::AlignTop);

    presetCoordLayout->addWidget(new QLabel("Choose origo: "));
    presetOrigoComboBox = new QComboBox();
    initializeButton=new QPushButton();
    presetCoordLayout->addWidget(presetOrigoComboBox);
    presetCoordLayout->addWidget(initializeButton);

    presetOrigoComboBox->clear();
    presetOrigoComboBox->insertItems(0, QStringList()
                                     << QApplication::translate("Ur5Widget", "Buttom right corner", 0)
                                     << QApplication::translate("Ur5Widget", "Current position", 0)
                                     );

    initializeButton->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    QGridLayout *manualCoordLayout = new QGridLayout(manualCoordinatesTab);
    manualCoordinatesLineEdit=new QLineEdit();
    initializeButton_2=new QPushButton();
    manualCoordLayout->setAlignment(Qt::AlignTop);
    manualCoordLayout->addWidget(new QLabel("Set coordinates: "), 0, 0, 1, 1);
    manualCoordLayout->addWidget(manualCoordinatesLineEdit,0,1,1,1);
    manualCoordLayout->addWidget(initializeButton_2,0,2,1,1);

    manualCoordinatesLineEdit->setText(QApplication::translate("Ur5Widget", "(x,y,z,rx,ry,rz)", 0));
    initializeButton_2->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
    initializeButton_2->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

    row++;
    shutdownButton = new QPushButton();
    disconnectButton = new QPushButton();
    mainLayout->addWidget(shutdownButton,row,0,1,1);
    mainLayout->addWidget(disconnectButton,row,3,1,1);
    QIcon icon1;
    icon1.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/network-offline.ico"), QSize(), QIcon::Normal, QIcon::Off);
    disconnectButton->setIcon(icon1);
    disconnectButton->setText("Disconnect");
    QIcon icon2;
    icon2.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/system-log-out-5.ico"), QSize(), QIcon::Normal, QIcon::Off);
    shutdownButton->setIcon(icon2);
    shutdownButton->setText("Shutdown");
}

void Ur5InitializeTab::connectButtonSlot()
{
    mUr5Robot->setAddress(ipLineEdit->text());
    mUr5Robot->connectToRobot();

    if(mUr5Robot->isConnectedToRobot() && !connectButton->isChecked())
    {
        connectButton->toggle();
    }
    else if(!mUr5Robot->isConnectedToRobot() && connectButton->isChecked())
    {
        connectButton->toggle();
    }
}

void Ur5InitializeTab::disconnectButtonSlot()
{
    mUr5Robot->disconnectFromRobot();

    if(!mUr5Robot->isConnectedToRobot() && connectButton->isChecked())
        connectButton->toggle();

}

void Ur5InitializeTab::initializeButtonSlot()
{
    if(presetOrigoComboBox->currentText() == "Buttom right corner")
    {
        report("Moving to buttom right corner.");
        mUr5Robot->initializeWorkspace(0.00005,Ur5State(-0.36,-0.64,0.29,-1.87,-2.50,0),false);
    }
    else if(presetOrigoComboBox->currentText() == "Current position")
    {
       report("Setting current pos to origo");
       mUr5Robot->initializeWorkspace(0.000005,Ur5State(0,0,0,0,0,0),true);
    }
}

void Ur5InitializeTab::shutdownButtonSlot()
{
    mUr5Robot->shutdown();
}

} // cx

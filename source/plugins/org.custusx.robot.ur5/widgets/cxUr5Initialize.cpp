#include "cxUr5Initialize.h"
#include "cxLogger.h"


#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QApplication>
#include <QGroupBox>

namespace cx
{


Ur5InitializeTab::Ur5InitializeTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connect(connectButton,&QPushButton::clicked,this,&Ur5InitializeTab::connectButtonSlot);
    connect(disconnectButton,&QPushButton::clicked,this,&Ur5InitializeTab::disconnectButtonSlot);
    connect(shutdownButton,&QPushButton::clicked,this,&Ur5InitializeTab::shutdownButtonSlot);
    connect(startTrackingButton,&QPushButton::clicked,this,&Ur5InitializeTab::startTrackingSlot);
    connect(stopTrackingButton,&QPushButton::clicked,this,&Ur5InitializeTab::stopTrackingSlot);
}

Ur5InitializeTab::~Ur5InitializeTab()
{
}

void Ur5InitializeTab::setupUi(QWidget *parent)
{   
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setRobotConnectionLayout(mainLayout);
    setRobotTrackingLayout(mainLayout);

    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);

//    row ++;
//    mainLayout->addWidget(new QLabel("Set origo for robot workspace"),row,0,1,2);

//    row++;
//    QTabWidget *setCoordinatesTab = new QTabWidget();
//    QWidget *presetCoordinatesTab = new QWidget();
//    QWidget *manualCoordinatesTab = new QWidget();
//    setCoordinatesTab->addTab(presetCoordinatesTab,tr("Preset coordinates"));
//    setCoordinatesTab->addTab(manualCoordinatesTab,tr("Manual coordinates"));
//    mainLayout->addWidget(setCoordinatesTab,row,0,1,4);

//    QHBoxLayout *presetCoordLayout = new QHBoxLayout(presetCoordinatesTab);
//    presetCoordLayout->setAlignment(Qt::AlignTop);

//    presetCoordLayout->addWidget(new QLabel("Choose origo: "));
//    presetOrigoComboBox = new QComboBox();
//    initializeButton=new QPushButton();
//    presetCoordLayout->addWidget(presetOrigoComboBox);
//    presetCoordLayout->addWidget(initializeButton);

//    presetOrigoComboBox->clear();
//    presetOrigoComboBox->insertItems(0, QStringList()
//                                     << QApplication::translate("Ur5Widget", "Buttom right corner", 0)
//                                     << QApplication::translate("Ur5Widget", "Current position", 0)
//                                     );

//    initializeButton->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
//    initializeButton->setText(QApplication::translate("Ur5Widget", "Initialize", 0));

//    QGridLayout *manualCoordLayout = new QGridLayout(manualCoordinatesTab);
//    manualCoordinatesLineEdit=new QLineEdit();
//    initializeButton_2=new QPushButton();
//    manualCoordLayout->setAlignment(Qt::AlignTop);
//    manualCoordLayout->addWidget(new QLabel("Set coordinates: "), 0, 0, 1, 1);
//    manualCoordLayout->addWidget(manualCoordinatesLineEdit,0,1,1,1);
//    manualCoordLayout->addWidget(initializeButton_2,0,2,1,1);

//    manualCoordinatesLineEdit->setText(QApplication::translate("Ur5Widget", "(x,y,z,rx,ry,rz)", 0));
//    initializeButton_2->setToolTip(QApplication::translate("Ur5Widget", "Initialize workspace", 0));
//    initializeButton_2->setText(QApplication::translate("Ur5Widget", "Initialize", 0));


}

void Ur5InitializeTab::setRobotConnectionLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Robot connection");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    int row = 0;
    ipLineEdit = new QLineEdit();
    connectButton = new QPushButton();
    mainLayout->addWidget(new QLabel("IP Address: "), row, 0, 1, 1);
    mainLayout->addWidget(ipLineEdit, row, 1,1,2);
    mainLayout->addWidget(connectButton,row,3,1,1);

    ipLineEdit->setText("169.254.62.100");

    QIcon icon;
    icon.addFile("/icons/network-idle.ico", QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile("/icons/network-transmit-receive.ico", QSize(), QIcon::Normal, QIcon::On);
    connectButton->setIcon(icon);
    connectButton->setToolTip("Connect to robot");
    connectButton->setText("Connect");
    connectButton->setCheckable(true);
    connectButton->setStyleSheet("QPushButton:checked { background-color: none; }");

    row++;
    shutdownButton = new QPushButton(QIcon("/icons/application-exit-4.png"),"Shutdown");
    disconnectButton = new QPushButton(QIcon("/icons/network-offline.ico"),"Disconnect");
    mainLayout->addWidget(shutdownButton,row,0,1,1);
    mainLayout->addWidget(disconnectButton,row,3,1,1);
}

void Ur5InitializeTab::setRobotTrackingLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Robot Tracking");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    startTrackingButton = new QPushButton(tr("Start tracking"));
    stopTrackingButton = new QPushButton(tr("Stop tracking"));

    int row = 0;
//    mainLayout->addWidget(configureTrackingButton, row, 0,1,1);
//    mainLayout->addWidget(deconfigureTrackingButton,row,3,1,1);

//    row++;
//    mainLayout->addWidget(initializeTrackingButton,row,0,1,1);
//    mainLayout->addWidget(deinitializeTrackingButton,row,3,1,1);

//    row++;
    mainLayout->addWidget(startTrackingButton,row,0,1,1);
    mainLayout->addWidget(stopTrackingButton,row,3,1,1);
}

void Ur5InitializeTab::startTrackingSlot()
{
    mUr5Robot->startTracking();
}

void Ur5InitializeTab::stopTrackingSlot()
{
    mUr5Robot->stopTracking();
}


void Ur5InitializeTab::connectButtonSlot()
{
    mUr5Robot->connectToRobot(ipLineEdit->text());

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

void Ur5InitializeTab::shutdownButtonSlot()
{
    mUr5Robot->shutdown();
}

} // cx

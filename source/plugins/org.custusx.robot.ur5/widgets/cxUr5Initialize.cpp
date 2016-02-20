#include "cxUr5Initialize.h"
#include "cxLogger.h"


#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QApplication>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QDir>
#include <QSpacerItem>


namespace cx
{


Ur5InitializeTab::Ur5InitializeTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mGraphicsFolderName("/CX/source/plugins/org.custusx.robot.ur5/widgets/icons/")
{
    setupUi(this);
    setupConnections(this);
}

Ur5InitializeTab::~Ur5InitializeTab()
{
}

void Ur5InitializeTab::setupUi(QWidget *parent)
{   
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *secondaryWidget = new QWidget();
    QHBoxLayout *secondaryLayout = new QHBoxLayout(secondaryWidget);

    setRobotConnectionLayout(mainLayout);
    setRobotTrackingLayout(secondaryLayout);
    setRobotApplicationLayout(secondaryLayout);

    mainLayout->addWidget(secondaryWidget);
    mainLayout->addWidget(new QWidget());

    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);
}

void Ur5InitializeTab::setupConnections(QWidget *parent)
{
    connect(connectButton,&QPushButton::clicked,this,&Ur5InitializeTab::connectButtonSlot);
    connect(disconnectButton,&QPushButton::clicked,this,&Ur5InitializeTab::disconnectButtonSlot);
    connect(shutdownButton,&QPushButton::clicked,this,&Ur5InitializeTab::shutdownButtonSlot);

    connect(trackingButton,&QPushButton::clicked,this,&Ur5InitializeTab::trackingSlot);
    connect(linksButton, &QPushButton::clicked, this, &Ur5InitializeTab::robotLinkSlot);

    connect(toggleManual, &QPushButton::toggled, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(toggleLungSimulation, &QPushButton::toggled, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(togglePlanned, &QPushButton::toggled, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(toggleSettings, &QPushButton::toggled, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(toggleUr5Script, &QPushButton::toggled, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(toggleUSTracker, &QPushButton::toggled, this, &Ur5InitializeTab::addCheckedApplicationSlot);
}

void Ur5InitializeTab::setRobotConnectionLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Robot Connection");
    group->setFlat(true);
    parent->addWidget(group,0,Qt::AlignTop);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    int row = 0;
    ipLineEdit = new QLineEdit();
    connectButton = new QPushButton();
    mainLayout->addWidget(new QLabel("IP Address: "), row, 0, 1, 1);
    mainLayout->addWidget(ipLineEdit, row, 1,1,1);
    mainLayout->addWidget(connectButton,row,2,1,1);

    ipLineEdit->setText("10.218.140.144");
    ipLineEdit->setAlignment(Qt::AlignCenter);

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
    mainLayout->addWidget(disconnectButton,row,2,1,1);
}

void Ur5InitializeTab::setRobotTrackingLayout(QHBoxLayout *parent)
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();

    QIcon onoffIcon;
    onoffIcon.addFile(dir.path()+mGraphicsFolderName+"off.ico", QSize(), QIcon::Normal, QIcon::Off);
    onoffIcon.addFile(dir.path()+mGraphicsFolderName+"on.ico", QSize(), QIcon::Normal, QIcon::On);

    QGroupBox* group = new QGroupBox("Robot Tracking");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    trackingButton = new QPushButton(onoffIcon,"");
    trackingButton->setCheckable(true);
    trackingButton->setStyleSheet(this->onoffButtonStyleSheet());
    trackingButton->setFixedWidth(40);
    trackingButton->setFixedHeight(24);
    trackingButton->setIconSize(trackingButton->size());

    linksButton = new QPushButton(onoffIcon,"");
    linksButton->setCheckable(true);
    linksButton->setStyleSheet(this->onoffButtonStyleSheet());
    linksButton->setFixedWidth(40);
    linksButton->setFixedHeight(24);
    linksButton->setIconSize(trackingButton->size());

    int row = 0;
    mainLayout->addWidget(new QLabel(tr("Tracking")),row,0,1,1, Qt::AlignVCenter);
    mainLayout->addWidget(trackingButton,row,1,1,1, Qt::AlignVCenter);

    row++;
    mainLayout->addWidget(new QLabel(tr("Visualize links")),row,0,1,1, Qt::AlignVCenter);
    mainLayout->addWidget(linksButton,row,1,1,1, Qt::AlignVCenter);

    row++;
    mainLayout->addWidget(new QLabel(),row,0,1,1);

    row++;
    mainLayout->addWidget(new QLabel(),row,0,1,1);

    row++;
    mainLayout->addWidget(new QLabel(),row,0,1,1);

}

void Ur5InitializeTab::setRobotApplicationLayout(QHBoxLayout *parent)
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();

    QIcon onoffIcon;
    onoffIcon.addFile(dir.path()+mGraphicsFolderName+"off.ico", QSize(), QIcon::Normal, QIcon::Off);
    onoffIcon.addFile(dir.path()+mGraphicsFolderName+"on.ico", QSize(), QIcon::Normal, QIcon::On);

    QGroupBox* group = new QGroupBox("Robot Applications");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);


    toggleManual = new QPushButton(onoffIcon,"");
    toggleManual->setCheckable(true);
    toggleManual->setStyleSheet(this->onoffButtonStyleSheet());
    toggleManual->setFixedWidth(40);
    toggleManual->setFixedHeight(24);
    toggleManual->setIconSize(toggleManual->size());

    togglePlanned = new QPushButton(onoffIcon,"");
    togglePlanned->setCheckable(true);
    togglePlanned->setStyleSheet(this->onoffButtonStyleSheet());
    togglePlanned->setFixedWidth(40);
    togglePlanned->setFixedHeight(24);
    togglePlanned->setIconSize(toggleManual->size());

    toggleUr5Script = new QPushButton(onoffIcon,"");
    toggleUr5Script->setCheckable(true);
    toggleUr5Script->setStyleSheet(this->onoffButtonStyleSheet());
    toggleUr5Script->setFixedWidth(40);
    toggleUr5Script->setFixedHeight(24);
    toggleUr5Script->setIconSize(toggleManual->size());

    toggleLungSimulation = new QPushButton(onoffIcon,"");
    toggleLungSimulation->setCheckable(true);
    toggleLungSimulation->setStyleSheet(this->onoffButtonStyleSheet());
    toggleLungSimulation->setFixedWidth(40);
    toggleLungSimulation->setFixedHeight(24);
    toggleLungSimulation->setIconSize(toggleManual->size());

    toggleSettings = new QPushButton(onoffIcon,"");
    toggleSettings->setCheckable(true);
    toggleSettings->setStyleSheet(this->onoffButtonStyleSheet());
    toggleSettings->setFixedWidth(40);
    toggleSettings->setFixedHeight(24);
    toggleSettings->setIconSize(toggleManual->size());

    toggleUSTracker = new QPushButton(onoffIcon,"");
    toggleUSTracker->setCheckable(true);
    toggleUSTracker->setStyleSheet(this->onoffButtonStyleSheet());
    toggleUSTracker->setFixedWidth(40);
    toggleUSTracker->setFixedHeight(24);
    toggleUSTracker->setIconSize(toggleManual->size());

    mainLayout->addWidget(new QLabel(tr("Manual movement")),0,0,1,1);
    mainLayout->addWidget(toggleManual,0,1,1,1);

    mainLayout->addWidget(new QLabel(tr("Planned movement")),1,0,1,1);
    mainLayout->addWidget(togglePlanned,1,1,1,1);

    mainLayout->addWidget(new QLabel(tr("UR5 Script")),2,0,1,1);
    mainLayout->addWidget(toggleUr5Script,2,1,1,1);

    mainLayout->addWidget(new QLabel(tr("Lung Simulation")),3,0,1,1);
    mainLayout->addWidget(toggleLungSimulation,3,1,1,1);

    mainLayout->addWidget(new QLabel(tr("Settings")),4,0,1,1);
    mainLayout->addWidget(toggleSettings,4,1,1,1);

    mainLayout->addWidget(new QLabel(tr("US Tracker")),5,0,1,1);
    mainLayout->addWidget(toggleUSTracker,5,1,1,1);
}

void Ur5InitializeTab::startTrackingSlot()
{
    mUr5Robot->startTracking();

    mUr5Robot->transform("RobotTracker",mUr5Robot->getCurrentState().bMee,0); // Bør fjærnes
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

void Ur5InitializeTab::addRobotLinkSlot()
{
    mUr5Robot->addRobotVisualizationLinks();
}

void Ur5InitializeTab::removeRobotLinkSlot()
{
    mUr5Robot->removeRobotVisualizationLinks();
}

void Ur5InitializeTab::addCheckedApplicationSlot()
{
    if(toggleManual->isChecked())
    {
        this->addApplicationTab("Manual");
    }
    else
    {
        this->removeApplicationTab("Manual");
    }

    if(togglePlanned->isChecked())
    {
        this->addApplicationTab("Planned");
    }
    else
    {
        this->removeApplicationTab("Planned");
    }

    if(toggleUr5Script->isChecked())
    {
        this->addApplicationTab("UR5Script");
    }
    else
    {
        this->removeApplicationTab("UR5Script");
    }

    if(toggleLungSimulation->isChecked())
    {
        this->addApplicationTab("LungSimulation");
    }
    else
    {
        this->removeApplicationTab("LungSimulation");
    }

    if(toggleSettings->isChecked())
    {
        this->addApplicationTab("Settings");
    }
    else
    {
        this->removeApplicationTab("Settings");
    }

    if(toggleUSTracker->isChecked())
    {
        this->addApplicationTab("USTracker");
    }
    else
    {
        this->removeApplicationTab("USTracker");
    }
}

QString Ur5InitializeTab::onoffButtonStyleSheet()
{
    QString str = "QPushButton{background-color: transparent; outline: none} /";

    return str;
}

void Ur5InitializeTab::trackingSlot()
{
    if(trackingButton->isChecked())
    {
        mUr5Robot->startTracking();
        mUr5Robot->transform("RobotTracker",mUr5Robot->getCurrentState().bMee,0); // Bør fjærnes
    }
    else
    {
        mUr5Robot->stopTracking();
    }
}

void Ur5InitializeTab::robotLinkSlot()
{
    if(linksButton->isChecked())
    {
        mUr5Robot->addRobotVisualizationLinks();
    }
    else
    {
        mUr5Robot->removeRobotVisualizationLinks();
    }

}

} // cx

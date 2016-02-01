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


namespace cx
{


Ur5InitializeTab::Ur5InitializeTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
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

    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);
}

void Ur5InitializeTab::setupConnections(QWidget *parent)
{
    connect(connectButton,&QPushButton::clicked,this,&Ur5InitializeTab::connectButtonSlot);
    connect(disconnectButton,&QPushButton::clicked,this,&Ur5InitializeTab::disconnectButtonSlot);
    connect(shutdownButton,&QPushButton::clicked,this,&Ur5InitializeTab::shutdownButtonSlot);
    connect(startTrackingButton,&QPushButton::clicked,this,&Ur5InitializeTab::startTrackingSlot);
    connect(stopTrackingButton,&QPushButton::clicked,this,&Ur5InitializeTab::stopTrackingSlot);
    connect(addLinksButton, &QPushButton::clicked, this, &Ur5InitializeTab::addRobotLinkSlot);
    connect(removeLinksButton, &QPushButton::clicked, this, &Ur5InitializeTab::removeRobotLinkSlot);

    connect(toggleManual, &QCheckBox::stateChanged, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(toggleLungSimulation, &QCheckBox::stateChanged, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(togglePlanned, &QCheckBox::stateChanged, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(toggleSettings, &QCheckBox::stateChanged, this, &Ur5InitializeTab::addCheckedApplicationSlot);
    connect(toggleUr5Script, &QCheckBox::stateChanged, this, &Ur5InitializeTab::addCheckedApplicationSlot);
}

void Ur5InitializeTab::setRobotConnectionLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Robot Connection");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    int row = 0;
    ipLineEdit = new QLineEdit();
    connectButton = new QPushButton();
    mainLayout->addWidget(new QLabel("IP Address: "), row, 0, 1, 1);
    mainLayout->addWidget(ipLineEdit, row, 1,1,1);
    mainLayout->addWidget(connectButton,row,2,1,1);

    ipLineEdit->setText("10.218.140.144");

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
    QGroupBox* group = new QGroupBox("Robot Tracking");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    startTrackingButton = new QPushButton(tr("Start tracking"));
    stopTrackingButton = new QPushButton(tr("Stop tracking"));
    addLinksButton = new QPushButton(tr("Add links"));
    removeLinksButton = new QPushButton(tr("Remove links"));

    int row = 0;
    mainLayout->addWidget(startTrackingButton,row,0,1,1);
    mainLayout->addWidget(stopTrackingButton,row,1,1,1);

    row++;
    mainLayout->addWidget(addLinksButton,row,0,1,1);
    mainLayout->addWidget(removeLinksButton,row,1,1,1);
}

void Ur5InitializeTab::setRobotApplicationLayout(QHBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Robot Applications");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    toggleManual = new QCheckBox(tr("Manual movement"));
    togglePlanned = new QCheckBox(tr("Planned movement"));
    toggleUr5Script = new QCheckBox(tr("UR5 Script"));
    toggleLungSimulation = new QCheckBox(tr("Lung simulation"));
    toggleSettings = new QCheckBox(tr("Settings"));

    mainLayout->addWidget(toggleManual,0,1,1,1);
    mainLayout->addWidget(togglePlanned,1,1,1,1);
    mainLayout->addWidget(toggleUr5Script,2,1,1,1);
    mainLayout->addWidget(toggleLungSimulation,3,1,1,1);
    mainLayout->addWidget(toggleSettings,4,1,1,1);
}

void Ur5InitializeTab::startTrackingSlot()
{
    mUr5Robot->startTracking();

    Transform3D trackingMatrix = Transform3D(mUr5Robot->getCurrentState().bMee); // Fjærnes
    trackingMatrix.translation() = trackingMatrix.translation()*1000; // Fjærnes
    mUr5Robot->transform("RobotTracker",trackingMatrix,0); // Bør fjærnes
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
}

} // cx

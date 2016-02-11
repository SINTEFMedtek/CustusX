#include "cxUr5Misc.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QGroupBox>
#include <QList>
#include <cxVisServices.h>
#include <cxTrackingService.h>
#include "trackingSystemRobot/cxRobotTool.h"

#include "cxLogger.h"


namespace cx
{

Ur5MiscellaneousTab::Ur5MiscellaneousTab(Ur5RobotPtr Ur5Robot,VisServicesPtr services, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mServices(services)
{
    setupUi(this);

    connect(startLoggingButton, &QPushButton::clicked, this, &Ur5MiscellaneousTab::startLoggingSlot);
    connect(stopLoggingButton, &QPushButton::clicked, this, &Ur5MiscellaneousTab::stopLoggingSlot);
}

Ur5MiscellaneousTab::~Ur5MiscellaneousTab()
{
}

void Ur5MiscellaneousTab::setupUi(QWidget *parent)
{
   mainLayout = new QHBoxLayout(this);

   QWidget *leftColumnWidgets = new QWidget();
   QVBoxLayout *leftColumnLayout = new QVBoxLayout(leftColumnWidgets);

   setLoggingLayout(leftColumnLayout);

   mainLayout->addWidget(leftColumnWidgets,0,Qt::AlignTop|Qt::AlignLeft);

   mainLayout->setSpacing(5);
   mainLayout->setMargin(5);
}

void Ur5MiscellaneousTab::setLoggingLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Setup logging");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *loggingLayout = new QGridLayout();
    group->setLayout(loggingLayout);
}

void Ur5MiscellaneousTab::startLoggingSlot()
{
    connect(mUr5Robot.get(),&Ur5Robot::stateUpdated,this,&Ur5MiscellaneousTab::dataLogger);
    CX_LOG_INFO() << "Logging started";
}

void Ur5MiscellaneousTab::stopLoggingSlot()
{
    disconnect(mUr5Robot.get(),&Ur5Robot::stateUpdated,this,&Ur5MiscellaneousTab::dataLogger);
    CX_LOG_INFO() << "Logging stopped";
}

void Ur5MiscellaneousTab::dataLogger()
{
     CX_LOG_CHANNEL_INFO("jointConfiguration") << mUr5Robot->getCurrentState().jointConfiguration;
     CX_LOG_CHANNEL_INFO("jointVelocitites") << mUr5Robot->getCurrentState().jointVelocity;
     CX_LOG_CHANNEL_INFO("operationalPosition") << mUr5Robot->getCurrentState().cartAxis;
     CX_LOG_CHANNEL_INFO("operationalVelocity") << mUr5Robot->getCurrentState().operationalVelocity;
}


} // cx

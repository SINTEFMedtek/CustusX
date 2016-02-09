#include "cxUr5Misc.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QGroupBox>
#include <QList>
#include <cxVisServices.h>
#include <cxTrackingService.h>
#include "trackingSystemRobot/cxRobotTool.h"


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
    mUr5Robot->startLogging();
}

void Ur5MiscellaneousTab::stopLoggingSlot()
{
    mUr5Robot->stopLogging();
}


} // cx

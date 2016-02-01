#include "cxUr5Settings.h"

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

Ur5SettingsTab::Ur5SettingsTab(Ur5RobotPtr Ur5Robot,VisServicesPtr services, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mServices(services)
{
    setupUi(this);

    updateCombobox();

    connect(autoCalibrateButton, &QPushButton::clicked, this, &Ur5SettingsTab::autoCalibrateSlot);
}

Ur5SettingsTab::~Ur5SettingsTab()
{
}

void Ur5SettingsTab::setupUi(QWidget *parent)
{
   mainLayout = new QHBoxLayout(this);

   QWidget *leftColumnWidgets = new QWidget();
   QVBoxLayout *leftColumnLayout = new QVBoxLayout(leftColumnWidgets);

   setToolConfigurationLayout(leftColumnLayout);

   mainLayout->addWidget(leftColumnWidgets,0,Qt::AlignTop|Qt::AlignLeft);

   mainLayout->setSpacing(5);
   mainLayout->setMargin(5);
}

void Ur5SettingsTab::setToolConfigurationLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Tool Configuration");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *keyLayout = new QGridLayout();
    group->setLayout(keyLayout);

    keyLayout->addWidget(new QLabel("Select tool:"), 0,0, 1, 1, Qt::AlignHCenter);

    toolComboBox = new QComboBox;
    keyLayout->addWidget(toolComboBox,0,1,1,1);

    autoCalibrateButton = new QPushButton(tr("Auto calibrate"));
    keyLayout->addWidget(autoCalibrateButton,0,2,1,1);
}

void Ur5SettingsTab::updateCombobox()
{
    ToolMap tools = mServices->tracking()->getTools();
    for (TrackingService::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
        toolComboBox->addItem(iter->second->getUid());
}

void Ur5SettingsTab::autoCalibrateSlot()
{
    ToolPtr tool = mServices->tracking()->getTool("RobotTracker");
    RobotToolPtr robotTool = boost::dynamic_pointer_cast<RobotTool>(tool);

    createCalibrationMatrix();
}

void Ur5SettingsTab::createCalibrationMatrix()
{
    Transform3D calMatrix = Transform3D::Identity();

    ToolPtr rtool = mServices->tracking()->getTool("RobotTracker");
    RobotToolPtr robotTool = boost::dynamic_pointer_cast<RobotTool>(rtool);

    ToolPtr tool = mServices->tracking()->getTool("ManualTool");

    //std::cout << tool->get_prMt().inverse() << std::endl;
    //std::cout << robotTool->get_prMb() << std::endl;
    //std::cout << mUr5Robot->getCurrentState().bMee << std::endl;
}

} // cx

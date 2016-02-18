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
    connect(clearCalibrationButton, &QPushButton::clicked, this, &Ur5SettingsTab::clearCalibrationSlot);

    connect(logForCalibrationButton, &QPushButton::clicked, this, &Ur5SettingsTab::logForCalibrationSlot);
    connect(clearLogForCalibrationButton, &QPushButton::clicked, this, &Ur5SettingsTab::clearLogForCalibrationSlot);
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

    logForCalibrationButton = new QPushButton(tr("Log transformations"));
    keyLayout->addWidget(logForCalibrationButton,1,0,1,1);

    clearLogForCalibrationButton = new QPushButton(tr("Clear Log"));
    keyLayout->addWidget(clearLogForCalibrationButton,1,1,1,1);

    autoCalibrateButton = new QPushButton(tr("Calibrate"));
    keyLayout->addWidget(autoCalibrateButton,2,0,1,1);

    clearCalibrationButton = new QPushButton(tr("Clear calibration"));
    keyLayout->addWidget(clearCalibrationButton,2,1,1,1);
}

void Ur5SettingsTab::updateCombobox()
{
    ToolMap tools = mServices->tracking()->getTools();
    for (TrackingService::ToolMap::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
        toolComboBox->addItem(iter->second->getUid());
}

void Ur5SettingsTab::autoCalibrateSlot()
{
    if(!_prMt.empty() && !_bMe.empty())
    {
        this->createCalibrationMatrices();
    }
    else
    {
        mUr5Robot->set_prMb(getPreCalibrated_prMb());
        mUr5Robot->set_eMt(getPreCalibrated_eMt());
    }

}

void Ur5SettingsTab::createCalibrationMatrix()
{
    Transform3D calMatrix = Transform3D::Identity();

    ToolPtr rtool = mServices->tracking()->getTool("RobotTracker");
    RobotToolPtr robotTool = boost::dynamic_pointer_cast<RobotTool>(rtool);

    ToolPtr tool = mServices->tracking()->getTool(toolComboBox->currentText());

    calMatrix = (tool->get_prMt().inverse()*robotTool->get_prMb()*mUr5Robot->getCurrentState().bMee).inverse();

    std::cout << calMatrix << std::endl;

    mUr5Robot->set_eMt(calMatrix);
}

void Ur5SettingsTab::createCalibrationMatrices()
{
    std::vector<Transform3D> _eMb, _tMpr;
    _eMb = Ur5Kinematics::invertMatrices(_bMe);
    _tMpr = Ur5Kinematics::invertMatrices(_prMt);

    mUr5Robot->set_eMt(Ur5Kinematics::calibrate_iMk(_eMb,_tMpr));
    mUr5Robot->set_prMb(Ur5Kinematics::calibrate_iMk(_prMt,_bMe));
}

void Ur5SettingsTab::clearCalibrationSlot()
{
    mUr5Robot->set_eMt(Transform3D::Identity());
}

void Ur5SettingsTab::logForCalibrationSlot()
{
    ToolPtr tool = mServices->tracking()->getTool(toolComboBox->currentText());

    _prMt.push_back(tool->get_prMt());
    _bMe.push_back(mUr5Robot->getCurrentState().bMee);
}

void Ur5SettingsTab::clearLogForCalibrationSlot()
{
    _prMt.clear();
    _bMe.clear();
}

Transform3D Ur5SettingsTab::getPreCalibrated_prMb()
{
    Eigen::Matrix4d prMb;

    prMb <<  0.0434563, -0.99905, -0.00326434, 44.1563,
             0.999052,   0.0434477,  0.00265485,     301.521,
            -0.00251049, -0.00337662 ,   0.999991,    -26.5665,
                     0,           0,           0,           1;

    return Transform3D(prMb);
}

Transform3D Ur5SettingsTab::getPreCalibrated_eMt()
{
    Eigen::Matrix4d eMt;

    eMt << 0.999292, -0.0361184, -0.010557,  -208.438,
           0.036101,  0.999346, -0.00182802, -3.54387,
           0.0106161,  0.0014456,    0.999943,     106.812,
                  0,           0,           0,           1;

    return Transform3D(eMt);
}

} // cx

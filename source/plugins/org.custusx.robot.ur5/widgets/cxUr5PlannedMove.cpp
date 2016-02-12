#include "cxUr5PlannedMove.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>
#include <QDir>

#include "cxLogger.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxSpaceProvider.h"
#include "cxSessionStorageService.h"
#include "cxPointMetric.h"
#include "cxFrameMetric.h"
#include "cxViewService.h"
#include "cxTrackingService.h"
#include "trackingSystemRobot/cxRobotTool.h"
#include "cxUr5Kinematics.h"
#include "cxTool.h"
#include "cxUr5ProgramEncoder.h"


namespace cx
{

Ur5PlannedMoveTab::Ur5PlannedMoveTab(Ur5RobotPtr Ur5Robot,VisServicesPtr services, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mServices(services)
{
    setupUi(this);

    connect(moveToPointButton, &QPushButton::clicked, this, &Ur5PlannedMoveTab::moveToPointSlot);
    connect(moveToFrameButton, &QPushButton::clicked, this, &Ur5PlannedMoveTab::moveToFrameSlot);
    connect(followActiveToolButton, &QPushButton::clicked, this, &Ur5PlannedMoveTab::followActiveToolSlot);

    connect(openVTKButton, &QPushButton::clicked, this, &Ur5PlannedMoveTab::openVTKfileSlot);
    connect(runVTKButton, &QPushButton::clicked, this, &Ur5PlannedMoveTab::runVTKfileSlot);
    connect(runVelocityVTKButton, &QPushButton::clicked, this, &Ur5PlannedMoveTab::runVelocityVTKSlot);
    connect(blendRadiusLineEdit, &QLineEdit::textChanged, this, &Ur5PlannedMoveTab::blendRadiusChangedSlot);
}

Ur5PlannedMoveTab::~Ur5PlannedMoveTab()
{

}

void Ur5PlannedMoveTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    setMovementAssignmentWidget(mainLayout);
    setMoveVTKWidget(mainLayout);
    setMoveSettingsWidget(mainLayout);
}

void Ur5PlannedMoveTab::blendRadiusChangedSlot()
{
    mUr5Robot->setBlendRadius(blendRadiusLineEdit->text().toDouble());
}

void Ur5PlannedMoveTab::setMoveVTKWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Follow .vtk line");
    group->setFlat(true);
    parent->addWidget(group);

    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *layout1 = new QHBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QHBoxLayout *layout4 = new QHBoxLayout();
    group->setLayout(vLayout);

    vLayout->addLayout(layout1);
    vLayout->addLayout(layout2);
    vLayout->addLayout(layout3);
    vLayout->addLayout(layout4);

    vtkLineEdit = new QLineEdit();
    runVTKButton = new QPushButton(tr("Run P2P Profile"));
    openVTKButton = new QPushButton(tr("Open"));
    runVelocityVTKButton = new QPushButton(tr("Run Velocity Profile"));
    clearPoseQueueButton = new QPushButton(tr("Clear pose queue"));
    moveToInitialPositionButton = new QPushButton(tr("Move to start"));

    layout1->addWidget(new QLabel("Path to .vtk file: "));

    layout1->addWidget(vtkLineEdit);
    layout1->addWidget(openVTKButton);
    layout2->addWidget(runVTKButton);
    layout2->addWidget(runVelocityVTKButton);

    runVTKButton->setToolTip("Follow VTK line");
    vtkLineEdit->setText("testing/testdata/line200.vtk");
}

void Ur5PlannedMoveTab::setMoveSettingsWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move settings");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *velAccLayout = new QGridLayout();
    group->setLayout(velAccLayout);

    velAccLayout->setSpacing(5);
    velAccLayout->setMargin(5);

    // Velocity
    velAccLayout->addWidget(new QLabel("Velocity"), 0, 0, 1, 1);
    velocityLineEdit = new QLineEdit();
    velAccLayout->addWidget(velocityLineEdit, 0, 1, 1, 1);
    velocityLineEdit->setText(QApplication::translate("Ur5Widget", "15", 0));
    velocityLineEdit->setAlignment(Qt::AlignRight);
    velAccLayout->addWidget(new QLabel("mm/s"), 0, 2, 1, 1);

    // Acceleration
    accelerationLineEdit = new QLineEdit();
    velAccLayout->addWidget(accelerationLineEdit, 1, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "300", 0));
    accelerationLineEdit->setAlignment(Qt::AlignRight);
    velAccLayout->addWidget(new QLabel("Acceleration"), 1, 0, 1, 1);
    velAccLayout->addWidget(new QLabel("mm/s^2"), 1, 2, 1, 1);

    // Blend radius
    blendRadiusLineEdit = new QLineEdit();
    blendRadiusLineEdit->setText(tr("1"));
    blendRadiusLineEdit->setAlignment(Qt::AlignRight);
    velAccLayout->addWidget(new QLabel("Blend radius"));
    velAccLayout->addWidget(blendRadiusLineEdit,2,1,1,1);
    velAccLayout->addWidget(new QLabel("mm"));
}

void Ur5PlannedMoveTab::setMovementAssignmentWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Assign movement");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *textEditLayout = new QGridLayout();
    group->setLayout(textEditLayout);

    moveToPointButton = new QPushButton(tr("Move to point"));
    textEditLayout->addWidget(moveToPointButton,0,0,1,1);

    moveToFrameButton = new QPushButton(tr("Move to frame"));
    textEditLayout->addWidget(moveToFrameButton,0,1,1,1);

    followActiveToolButton = new QPushButton(tr("Follow active tool"));
    textEditLayout->addWidget(followActiveToolButton,1,0,1,2);
    followActiveToolButton->setCheckable(true);
}

void Ur5PlannedMoveTab::runVTKfileSlot()
{
    mMovementQueue = Ur5ProgramEncoder::addMovementSettings(mMovementQueue, accelerationLineEdit->text().toDouble(),
                                                            velocityLineEdit->text().toDouble());
    mMovementQueue = Ur5ProgramEncoder::addTypeOfMovement(mMovementQueue, Ur5MovementInfo::movej);
    mUr5Robot->moveProgram(mMovementQueue);
    mMovementQueue.clear();
}

void Ur5PlannedMoveTab::runVelocityVTKSlot()
{
    mMovementQueue = Ur5ProgramEncoder::addMovementSettings(mMovementQueue, accelerationLineEdit->text().toDouble(),
                                                            velocityLineEdit->text().toDouble(),10);
    mMovementQueue = Ur5ProgramEncoder::addTypeOfMovement(mMovementQueue, Ur5MovementInfo::speedj);
    mUr5Robot->moveProgram(mMovementQueue);
    mMovementQueue.clear();
}

void Ur5PlannedMoveTab::openVTKfileSlot()
{
    mMovementQueue.clear();
    mMovementQueue = Ur5ProgramEncoder::createMovementQueueFromVTKFile(getPathToPlugin()+vtkLineEdit->text());
    mMovementQueue = Ur5ProgramEncoder::setConstantOrientation(mMovementQueue, mUr5Robot->getCurrentState().bMee);
}

void Ur5PlannedMoveTab::clearPoseQueueSlot()
{
    CX_LOG_DEBUG() << "Enters clear queue";
    mUr5Robot->clearMovementQueue();
}

void Ur5PlannedMoveTab::moveToPointSlot()
{
    DataPtr data = mServices->patient()->getData("point1");

    PointMetricPtr pointMetric = boost::dynamic_pointer_cast<PointMetric>(data);

    ToolPtr tool = mServices->tracking()->getTool("RobotTracker");
    RobotToolPtr robotTool = boost::dynamic_pointer_cast<RobotTool>(tool);

    Transform3D prMt_d = Transform3D::Identity();
    prMt_d.translation() = pointMetric->getCoordinate();

    Transform3D bMt_d = robotTool->get_prMb().inverse()*prMt_d;
    Vector3D rangles = Ur5Kinematics::T2rangles(mUr5Robot->getCurrentState().bMee*mUr5Robot->get_eMt());

    Eigen::RowVectorXd point(6);

    point << bMt_d(0,3), bMt_d(1,3), bMt_d(2,3),
            rangles(0), rangles(1), rangles(2);

    mUr5Robot->move("movejp",point,0.3,0.1);
}

void Ur5PlannedMoveTab::moveToFrameSlot()
{
    DataPtr data = mServices->patient()->getData("frame1");
    FrameMetricPtr frameMetric = boost::dynamic_pointer_cast<FrameMetric>(data);

    ToolPtr tool = mServices->tracking()->getTool("RobotTracker");
    RobotToolPtr robotTool = boost::dynamic_pointer_cast<RobotTool>(tool);

    Transform3D pose = (robotTool->get_prMb().inverse()*frameMetric->getFrame());

    mUr5Robot->move("movejp",Ur5Kinematics::T2OperationalConfiguration(pose),0.3,0.1);
}

void Ur5PlannedMoveTab::followActiveToolSlot()
{
    if(followActiveToolButton->isChecked())
    {
        connect(mServices->tracking()->getActiveTool().get(), &Tool::toolTransformAndTimestamp,
                this, &Ur5PlannedMoveTab::startFollowingActiveToolSlot);
    }
    else
    {
        disconnect(mServices->tracking()->getActiveTool().get(), &Tool::toolTransformAndTimestamp,
                   this, &Ur5PlannedMoveTab::startFollowingActiveToolSlot);
        mUr5Robot->stopMove("stopj",accelerationLineEdit->text().toDouble());
    }
}

void Ur5PlannedMoveTab::startFollowingActiveToolSlot(Transform3D matrix, double timestamp)
{
    ToolPtr tool = mServices->tracking()->getTool("RobotTracker");
    RobotToolPtr robotTool = boost::dynamic_pointer_cast<RobotTool>(tool);
    Transform3D currentToolPose = mUr5Robot->getCurrentState().bMee*mUr5Robot->get_eMt();

    if((Ur5Kinematics::T2transl((robotTool->get_prMb().inverse()*matrix))-Ur5Kinematics::T2transl(currentToolPose)).length()>blendRadiusLineEdit->text().toDouble())
    {

    double moveVelocity = velocityLineEdit->text().toDouble()/1000;


    Vector3D tangent = Ur5Kinematics::T2transl(robotTool->get_prMb().inverse()*matrix)-Ur5Kinematics::T2transl(currentToolPose);


    Eigen::RowVectorXd velocityEndEffector(6), jointVelocity(6);
    velocityEndEffector << moveVelocity*tangent(0)/tangent.norm(),moveVelocity*tangent(1)/tangent.norm(),moveVelocity*tangent(2)/tangent.norm()
                            ,0,0,0;

    jointVelocity = mUr5Robot->getCurrentState().jacobian.inverse()*velocityEndEffector.transpose();

    mUr5Robot->move("speedj",jointVelocity,accelerationLineEdit->text().toDouble()/1000,0,1,0);
    }
}

void Ur5PlannedMoveTab::startLoggingActiveTool(Transform3D matrix, double timestamp)
{

}

void Ur5PlannedMoveTab::runLoggedActiveToolSequence()
{

}

QString Ur5PlannedMoveTab::getPathToPlugin()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();

    QString pluginFolder = "/CX/source/plugins/org.custusx.robot.ur5/";

    return (dir.path() + pluginFolder);
}

} // cx

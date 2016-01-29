#include "cxUr5PlannedMove.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>

#include "cxLogger.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxSpaceProvider.h"
#include "cxSessionStorageService.h"
#include "cxPointMetric.h"
#include "cxViewService.h"
#include "cxTrackingService.h"
#include "trackingSystemRobot/cxRobotTool.h"


namespace cx
{

Ur5PlannedMoveTab::Ur5PlannedMoveTab(Ur5RobotPtr Ur5Robot,VisServicesPtr services, QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot),
    mServices(services)
{
    setupUi(this);

    connect(openVTKButton,SIGNAL(clicked()),this, SLOT(openVTKfileSlot()));
    connect(runVTKButton,SIGNAL(clicked()),this,SLOT(runVTKfileSlot()));
    connect(runVelocityVTKButton,SIGNAL(clicked()),this,SLOT(runVelocityVTKSlot()));
    connect(blendRadiusLineEdit,SIGNAL(textChanged(QString)),this,SLOT(blendRadiusChangedSlot()));
    connect(startLoggingButton,SIGNAL(clicked()),this,SLOT(startLoggingSlot()));
    connect(stopLoggingButton,SIGNAL(clicked()),this,SLOT(stopLoggingSlot()));
    connect(clearPoseQueueButton,SIGNAL(clicked()),this,SLOT(clearPoseQueueSlot()));

    connect(sendMessageButton,&QPushButton::clicked,this,&Ur5PlannedMoveTab::sendMessageSlot);
    connect(moveToInitialPositionButton,SIGNAL(clicked()),this,SLOT(moveToInitialPositionButtonSlot()));

    connect(getActiveLandmarkButton, &QPushButton::clicked, this, &Ur5PlannedMoveTab::getActiveLandmarkSlot);
}

Ur5PlannedMoveTab::~Ur5PlannedMoveTab()
{

}

void Ur5PlannedMoveTab::sendMessageSlot()
{
    mUr5Robot->sendMessage(textEditor->toPlainText());
}

void Ur5PlannedMoveTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);

    setMoveVTKWidget(mainLayout);
    setMoveSettingsWidget(mainLayout);
    setTextEditorWidget(mainLayout);
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
    startLoggingButton = new QPushButton(tr("Start logging"));
    stopLoggingButton = new QPushButton(tr("Stop logging"));
    clearPoseQueueButton = new QPushButton(tr("Clear pose queue"));
    moveToInitialPositionButton = new QPushButton(tr("Move to start"));

    layout1->addWidget(new QLabel("Path to .vtk file: "));

    layout1->addWidget(vtkLineEdit);
    layout1->addWidget(openVTKButton);
    layout2->addWidget(runVTKButton);
    layout2->addWidget(runVelocityVTKButton);
    //layout3->addWidget(startLoggingButton);
    //layout3->addWidget(stopLoggingButton);
    //layout4->addWidget(clearPoseQueueButton);
    //layout4->addWidget(moveToInitialPositionButton);

    runVTKButton->setToolTip("Follow VTK line");

    vtkLineEdit->setText("C:\\line.vtk");
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
    //velAccLayout->setContentsMargins(0,0,0,0);

    // Velocity
    velAccLayout->addWidget(new QLabel("Vel"), 0, 0, 1, 1);
    velocityLineEdit = new QLineEdit();
    velAccLayout->addWidget(velocityLineEdit, 0, 1, 1, 1);
    velocityLineEdit->setText(QApplication::translate("Ur5Widget", "0.015", 0));
    velAccLayout->addWidget(new QLabel("m/s"), 0, 2, 1, 1);

    // Acceleration
    accelerationLineEdit = new QLineEdit();
    velAccLayout->addWidget(accelerationLineEdit, 1, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "0.3", 0));
    velAccLayout->addWidget(new QLabel("Acc"), 1, 0, 1, 1);
    velAccLayout->addWidget(new QLabel("m/s^2"), 1, 2, 1, 1);

    // Blend radius
    blendRadiusLineEdit = new QLineEdit();
    blendRadiusLineEdit->setText(tr("0.001"));
    velAccLayout->addWidget(new QLabel("Blend radius"));
    velAccLayout->addWidget(blendRadiusLineEdit,2,1,1,1);
    velAccLayout->addWidget(new QLabel("m"));
}

void Ur5PlannedMoveTab::setTextEditorWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Ur5 Script");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *textEditLayout = new QGridLayout();
    group->setLayout(textEditLayout);

    textEditor = new QTextEdit();
    textEditLayout->addWidget(textEditor,0,0,2,2);
    textEditor->setText("movej([0.9019,-2.0358,2.0008,-1.5364,-1.5514,-3.6054],a=0.8,v=0.3)");

    sendMessageButton = new QPushButton(tr("Send message"));
    textEditLayout->addWidget(sendMessageButton,2,1,1,1);

    getActiveLandmarkButton = new QPushButton(tr("Get point"));
    textEditLayout->addWidget(getActiveLandmarkButton,2,0,1,1);
}

void Ur5PlannedMoveTab::runVTKfileSlot()
{
    mUr5Robot->moveProgram("movej",accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble(),0);
}

void Ur5PlannedMoveTab::runVelocityVTKSlot()
{
    CX_LOG_INFO() << "Starting velocity profile sequence";
    mUr5Robot->moveProgram("speedj",accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble(),0);
}

void Ur5PlannedMoveTab::openVTKfileSlot()
{
     mUr5Robot->openVTKfile(vtkLineEdit->text());
}

void Ur5PlannedMoveTab::startLoggingSlot()
{
    mUr5Robot->startLogging();
}

void Ur5PlannedMoveTab::stopLoggingSlot()
{
    mUr5Robot->stopLogging();
}

void Ur5PlannedMoveTab::clearPoseQueueSlot()
{
    CX_LOG_DEBUG() << "Enters clear queue";
    mUr5Robot->clearProgramQueue();
}

void Ur5PlannedMoveTab::moveToInitialPositionButtonSlot()
{
    mUr5Robot->moveToInitialPosition(accelerationLineEdit->text().toDouble(),velocityLineEdit->text().toDouble());
}

void Ur5PlannedMoveTab::getActiveLandmarkSlot()
{
    DataPtr data = mServices->patient()->getData("point1");

    PointMetricPtr pointMetric = boost::dynamic_pointer_cast<PointMetric>(data);
    //std::cout << pointMetric->getCoordinate() << std::endl;
    //std::cout << pointMetric->getSpace().toString() << std::endl;

    ToolPtr test1 = mServices->tracking()->getTool("RobotTracker");
    RobotToolPtr test = boost::dynamic_pointer_cast<RobotTool>(test1);

    Transform3D bMee = mUr5Robot->getCurrentState().bMee;
    bMee.translation() = bMee.translation()*1000;

    //std::cout << bMee << std::endl;
    //std::cout << test->get_eMt() << std::endl;
    //std::cout << test->get_prMb() << std::endl;

    Transform3D eMt = test->get_eMt();
    //eMt(2,3) = 10;

    Vector3D p = (eMt*test->get_prMb().inverse()*pointMetric->getCoordinate());
    Eigen::RowVectorXd point(6);
    point << p(0)/1000, p(1)/1000, p(2)/1000,
            mUr5Robot->getCurrentState().cartAngles(0), mUr5Robot->getCurrentState().cartAngles(1), mUr5Robot->getCurrentState().cartAngles(2);

    std::cout << point << std::endl;
    mUr5Robot->move("movejp",point,0.3,0.1);
}

} // cx

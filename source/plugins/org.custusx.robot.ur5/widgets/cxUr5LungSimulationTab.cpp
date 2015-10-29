#include "cxUr5LungSimulationTab.h"


#include "cxUr5MessageEncoder.h"

#include "cxLogger.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>


namespace cx
{

Ur5LungSimulationTab::Ur5LungSimulationTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connect(setStartPosButton, &QPushButton::clicked,this,&Ur5LungSimulationTab::setStartPosLineEdit);
    connect(setStopPosButton, &QPushButton::clicked,this,&Ur5LungSimulationTab::setStopPosLineEdit);
    connect(startMoveButton, &QPushButton::clicked,this,&Ur5LungSimulationTab::startSimulationSlot);
    connect(stopMoveButton, &QPushButton::clicked,this,&Ur5LungSimulationTab::stopSimulationSlot);

    mLungSimulation = new Ur5LungSimulation(mUr5Robot);
}

Ur5LungSimulationTab::~Ur5LungSimulationTab()
{

}

void Ur5LungSimulationTab::setupUi(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setSettingsLayout(mainLayout);
    setMoveLayout(mainLayout);
}

void Ur5LungSimulationTab::setSettingsLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Setup simulation");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    int row = 0;
    startPosLineEdit = new QLineEdit();
    setStartPosButton = new QPushButton(tr("Set position"));

    mainLayout->addWidget(new QLabel("Inspiration position: "), row, 0, 1, 1);
    mainLayout->addWidget(startPosLineEdit, row, 1,1,2);
    mainLayout->addWidget(setStartPosButton,row,3,1,1);

    row ++;
    stopPosLineEdit = new QLineEdit();
    setStopPosButton = new QPushButton(tr("Set position"));
    mainLayout->addWidget(new QLabel("Expiration position: "), row, 0, 1, 1);
    mainLayout->addWidget(stopPosLineEdit, row, 1,1,2);
    mainLayout->addWidget(setStopPosButton,row,3,1,1);

    row++;
    velocityProfileCBox = new QComboBox;
    velocityProfileCBox->addItem(tr("Trapezoidal"));
    mainLayout->addWidget(new QLabel("Velocity profile: "), row, 0, 1, 1);
    mainLayout->addWidget(velocityProfileCBox,row,1,1,1);

    row++;
    inspirationTimeLineEdit = new QLineEdit(tr("1.5"));
    mainLayout->addWidget(new QLabel("Inspiration time: "), row, 0, 1, 1);
    mainLayout->addWidget(inspirationTimeLineEdit,row,1,1,1);
    mainLayout->addWidget(new QLabel("s"), row, 2,1,1);

    row++;
    inspiratoryPauseTimeLineEdit = new QLineEdit(tr("1"));
    mainLayout->addWidget(new QLabel("Inspiratory pause time: "), row, 0, 1, 1);
    mainLayout->addWidget(inspiratoryPauseTimeLineEdit,row,1,1,1);
    mainLayout->addWidget(new QLabel("s"), row, 2,1,1);

    row++;
    expirationTimeLineEdit = new QLineEdit(tr("1"));
    mainLayout->addWidget(new QLabel("Expiration time: "), row, 0, 1, 1);
    mainLayout->addWidget(expirationTimeLineEdit,row,1,1,1);
    mainLayout->addWidget(new QLabel("s"),row,2,1,1);

    row++;
    expiratoryPauseTimeLineEdit = new QLineEdit(tr("2"));
    mainLayout->addWidget(new QLabel("Expiratory pause time: "), row, 0, 1, 1);
    mainLayout->addWidget(expiratoryPauseTimeLineEdit,row,1,1,1);
    mainLayout->addWidget(new QLabel("s"),row,2,1,1);

}

void Ur5LungSimulationTab::setMoveLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Run simulation");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *mainLayout = new QGridLayout();
    group->setLayout(mainLayout);

    int row = 0;
    startMoveButton = new QPushButton(tr("Start simulation"));
    mainLayout->addWidget(startMoveButton, row, 0, 1, 1);
    stopMoveButton = new QPushButton(tr("Stop simulation"));
    mainLayout->addWidget(stopMoveButton,row,1,1,1);
}

void Ur5LungSimulationTab::setStartPosLineEdit()
{
    jointStartPosition = mUr5Robot->getCurrentState().jointPosition;
    QString str("(");
    for(int i = 0; i<5; i++)
        str.append(QString::number(jointStartPosition(i))+",");
    startPosLineEdit->setText(str.append(QString::number(jointStartPosition(5))+")"));
}

void Ur5LungSimulationTab::setStopPosLineEdit()
{
    jointStopPosition = mUr5Robot->getCurrentState().jointPosition;
    QString str("(");
    for(int i = 0; i<5; i++)
        str.append(QString::number(jointStopPosition(i))+",");
    stopPosLineEdit->setText(str.append(QString::number(jointStopPosition(5))+")"));
}

void Ur5LungSimulationTab::startSimulationSlot()
{  
    mLungSimulation = new Ur5LungSimulation(mUr5Robot);
    mUr5Robot->clearProgramQueue();

    for(int i=0;i<500;i++)
    {
    mUr5Robot->addToProgramQueue(mMessageEncoder.movej(jointStartPosition,inspirationTimeLineEdit->text().toDouble()));
    mUr5Robot->addToProgramQueue(mMessageEncoder.movej(jointStopPosition,expirationTimeLineEdit->text().toDouble()));
    }

    mLungSimulation->lungMovement(inspirationTimeLineEdit->text().toDouble(),inspiratoryPauseTimeLineEdit->text().toDouble(),
                                  expirationTimeLineEdit->text().toDouble(),expiratoryPauseTimeLineEdit->text().toDouble());
}

void Ur5LungSimulationTab::stopSimulationSlot()
{
    mUr5Robot->clearProgramQueue();
    mLungSimulation->stopLungMovement();
    mUr5Robot->stopMove("stopj",0.5);
}

bool Ur5LungSimulationTab::isParametersSet()
{
    return(!(inspirationTimeLineEdit->text().isEmpty()) && !(expirationTimeLineEdit->text().isEmpty()) &&
           !(expiratoryPauseTimeLineEdit->text().isEmpty()) && !(inspiratoryPauseTimeLineEdit->text().isEmpty()) &&
           !(stopPosLineEdit->text().isEmpty()) && !(startPosLineEdit->text().isEmpty()));
}

} // cx


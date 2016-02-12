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

    connect(setInspirationPosButton, &QPushButton::clicked,this,&Ur5LungSimulationTab::setInspirationPosLineEdit);
    connect(setExpirationPosButton, &QPushButton::clicked,this,&Ur5LungSimulationTab::setExpirationPosLineEdit);
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
    inspirationPosLineEdit = new QLineEdit();
    setInspirationPosButton = new QPushButton(tr("Set position"));

    mainLayout->addWidget(new QLabel("Inspiration position: "), row, 0, 1, 1);
    mainLayout->addWidget(inspirationPosLineEdit, row, 1,1,2);
    mainLayout->addWidget(setInspirationPosButton,row,3,1,1);

    row ++;
    expirationPosLineEdit = new QLineEdit();
    setExpirationPosButton = new QPushButton(tr("Set position"));
    mainLayout->addWidget(new QLabel("Expiration position: "), row, 0, 1, 1);
    mainLayout->addWidget(expirationPosLineEdit, row, 1,1,2);
    mainLayout->addWidget(setExpirationPosButton,row,3,1,1);

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
    inspiratoryPauseTimeLineEdit = new QLineEdit(tr("0"));
    mainLayout->addWidget(new QLabel("Inspiratory pause time: "), row, 0, 1, 1);
    mainLayout->addWidget(inspiratoryPauseTimeLineEdit,row,1,1,1);
    mainLayout->addWidget(new QLabel("s"), row, 2,1,1);

    row++;
    expirationTimeLineEdit = new QLineEdit(tr("1.5"));
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

void Ur5LungSimulationTab::setInspirationPosLineEdit()
{
    inspirationState = mUr5Robot->getCurrentState();
    QString str("(");
    for(int i = 0; i<2; i++)
        str.append(QString::number(inspirationState.cartAxis(i))+",");
    inspirationPosLineEdit->setText(str.append(QString::number(inspirationState.cartAxis(2))+")"));
}

void Ur5LungSimulationTab::setExpirationPosLineEdit()
{
    expirationState = mUr5Robot->getCurrentState();
    QString str("(");
    for(int i = 0; i<2; i++)
        str.append(QString::number(expirationState.cartAxis(i))+",");
    expirationPosLineEdit->setText(str.append(QString::number(expirationState.cartAxis(2))+")"));
}

void Ur5LungSimulationTab::startSimulationSlot()
{  
    if(this->isParametersSet()
            && mUr5Robot->isValidWorkspace(inspirationState.jointConfiguration)
            && mUr5Robot->isValidWorkspace(expirationState.jointConfiguration))
    {
        mUr5Robot->clearMovementQueue();

        int nCycles = 500;
        MovementQueue mq(nCycles);

        for(int i=0;i<nCycles;i+=2)
        {
            mq.at(i).target_xMe = inspirationState.bMee;
            mq.at(i).time = inspirationTimeLineEdit->text().toDouble();
            mq.at(i).typeOfMovement = Ur5MovementInfo::movej;
            mq.at(i+1).target_xMe = expirationState.bMee;
            mq.at(i+1).time = expirationTimeLineEdit->text().toDouble();
            mq.at(i+1).typeOfMovement = Ur5MovementInfo::movej;
        }

        mLungSimulation->lungMovement(mq, inspiratoryPauseTimeLineEdit->text().toDouble(), expiratoryPauseTimeLineEdit->text().toDouble());
    }
    else
    {
        CX_LOG_INFO() << "All parameters not set or invalid positions.";
    }
}

void Ur5LungSimulationTab::stopSimulationSlot()
{
    mUr5Robot->clearMovementQueue();
    mLungSimulation->stopLungMovement();
    mUr5Robot->stopMove("stopj",0.5);
}

bool Ur5LungSimulationTab::isParametersSet()
{
    return(!(inspirationTimeLineEdit->text().isEmpty()) && !(expirationTimeLineEdit->text().isEmpty()) &&
           !(expiratoryPauseTimeLineEdit->text().isEmpty()) && !(inspiratoryPauseTimeLineEdit->text().isEmpty()) &&
           !(expirationPosLineEdit->text().isEmpty()) && !(inspirationPosLineEdit->text().isEmpty()));
}

} // cx


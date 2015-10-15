#include "cxUr5LungSimulation.h"

#include "cxLogger.h"

#include <QTimer>

namespace cx
{

Ur5LungSimulation::Ur5LungSimulation()
{

}

Ur5LungSimulation::Ur5LungSimulation(Ur5RobotPtr Ur5Robot) :
    mUr5Robot(Ur5Robot)
{
    connect(this,&Ur5LungSimulation::lungMovement,this,&Ur5LungSimulation::lungMovementSlot);
    connect(this,&Ur5LungSimulation::stopLungMovement,this,&Ur5LungSimulation::stopLungMove);
}

Ur5LungSimulation::~Ur5LungSimulation()
{

}

void Ur5LungSimulation::lungMovementSlot(double t1, double t2, double t3, double t4)
{
    mProgramQueue = mUr5Robot->getProgramQueue();

    mUr5Robot->sendMessage(mProgramQueue[0]);

    lungMovementTiming = Eigen::RowVectorXd(4);
    lungMovementTiming << t1, t2, t3, t4;

    this->startExpirationSequence();
    QTimer::singleShot((t3+t4)*1000,this,&Ur5LungSimulation::startInspirationSequence);

    CX_LOG_INFO() << "Lung simulation started";
}

void Ur5LungSimulation::startInspirationSequence()
{
    inspiration = new QTimer(this);
    connect(inspiration, &QTimer::timeout, this, &Ur5LungSimulation::continueLungMove);
    inspiration->start(lungMovementTiming.sum()*1000);
}

void Ur5LungSimulation::startExpirationSequence()
{
    expiration = new QTimer(this);
    connect(expiration,&QTimer::timeout,this,&Ur5LungSimulation::continueLungMove);
    expiration->start(lungMovementTiming.sum()*1000);
}

void Ur5LungSimulation::continueLungMove()
{
    mProgramQueue.erase(mProgramQueue.begin());

    if(mProgramQueue.empty())
    {
        inspiration->stop();
        expiration->stop();
        std::cout << "Empty programQueue" << std::endl;
    }
    else
    {
        mUr5Robot->sendMessage(mProgramQueue[0]);
    }
}

void Ur5LungSimulation::stopLungMove()
{
    inspiration->stop();
    expiration->stop();
    disconnect(inspiration, SIGNAL(timeout()), this, SLOT(continueLungMove()));
    disconnect(expiration,SIGNAL(timeout()),this,SLOT(continueLungMove()));
    mProgramQueue.clear();

    CX_LOG_INFO() << "Lung simulation stopped";
}

} // cx


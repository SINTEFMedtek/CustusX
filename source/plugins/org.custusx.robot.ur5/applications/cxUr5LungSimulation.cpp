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

    inspiration = new QTimer(this);
    expiration = new QTimer(this);
    expirationTiming = new QTimer(this);
    inspirationTiming = new QTimer(this);
}

Ur5LungSimulation::~Ur5LungSimulation()
{

}

void Ur5LungSimulation::lungMovementSlot(MovementQueue mq, double inspiratoryPauseTime, double expiratoryPauseTime)
{
    double inspirationTime = mq.front().time;
    double expirationTime = mq.at(1).time;

    mMovementQueue = mq;
    mUr5Robot->move(mq.front());

    lungMovementTiming = Eigen::RowVectorXd(4);
    lungMovementTiming << inspirationTime, inspiratoryPauseTime, expirationTime, expiratoryPauseTime;

    expirationTiming->setInterval((inspirationTime+inspiratoryPauseTime)*1000);
    expirationTiming->setSingleShot(true);
    connect(expirationTiming, SIGNAL(timeout()), SLOT(startExpirationSequence()));

    inspirationTiming->setInterval((inspirationTime+inspiratoryPauseTime+expirationTime+expiratoryPauseTime)*1000);
    inspirationTiming->setSingleShot(true);
    connect(inspirationTiming, SIGNAL(timeout()), SLOT(startInspirationSequence()));

    expirationTiming->start();
    inspirationTiming->start();

    CX_LOG_INFO() << "Moving to inspiratory position. Simulation will start in " << inspirationTime + inspiratoryPauseTime + lungMovementTiming.sum() << " seconds.";
}

void Ur5LungSimulation::startInspirationSequence()
{
    connect(inspiration,SIGNAL(timeout()), this, SLOT(continueLungMove()));
    inspiration->start(lungMovementTiming.sum()*1000);
}

void Ur5LungSimulation::startExpirationSequence()
{
    connect(expiration,SIGNAL(timeout()),this,SLOT(continueLungMove()));
    expiration->start(lungMovementTiming.sum()*1000);
}

void Ur5LungSimulation::continueLungMove()
{
    mMovementQueue.erase(mMovementQueue.begin());

    if(mMovementQueue.empty())
    {
        std::cout << "Empty programQueue" << std::endl;
        this->stopTimers();
    }
    else
    {
        mUr5Robot->move(mMovementQueue.front());
    }
}

void Ur5LungSimulation::stopLungMove()
{
    this->stopTimers();

    if(!mMovementQueue.empty())
        mMovementQueue.clear();

    CX_LOG_INFO() << "Lung simulation stopped";
}

void Ur5LungSimulation::stopTimers()
{
    inspirationTiming->stop();
    expirationTiming->stop();
    inspiration->stop();
    expiration->stop();

    disconnect(inspiration, SIGNAL(timeout()), this, SLOT(continueLungMove()));
    disconnect(expiration,SIGNAL(timeout()),this,SLOT(continueLungMove()));
}

} // cx


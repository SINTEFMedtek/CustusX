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

void Ur5LungSimulation::lungMovementSlot(double t1, double t2, double t3, double t4)
{
    mProgramQueue = mUr5Robot->getProgramQueue();

    mUr5Robot->sendMessage(mProgramQueue[0]);

    lungMovementTiming = Eigen::RowVectorXd(4);
    lungMovementTiming << t1, t2, t3, t4;

    expirationTiming->setInterval((t1+t2)*1000);
    expirationTiming->setSingleShot(true);
    connect(expirationTiming, SIGNAL(timeout()), SLOT(startExpirationSequence()));

    inspirationTiming->setInterval((t1+t2+t3+t4)*1000);
    inspirationTiming->setSingleShot(true);
    connect(inspirationTiming, SIGNAL(timeout()), SLOT(startInspirationSequence()));

    expirationTiming->start();
    inspirationTiming->start();

    CX_LOG_INFO() << "Moving to inspiratory position. Simulation will start in " << t1+t2+lungMovementTiming.sum() << " seconds.";
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
    mProgramQueue.erase(mProgramQueue.begin());

    if(mProgramQueue.empty())
    {
        inspirationTiming->stop();
        expirationTiming->stop();
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
    this->stopTimers();

    if(!mProgramQueue.empty())
        mProgramQueue.clear();

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


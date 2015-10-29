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

    QTimer::singleShot((t1+t2)*1000,this,SLOT(startExpirationSequence()));
    QTimer::singleShot((t1+t2+t3+t4)*1000,this,SLOT(startInspirationSequence()));

    CX_LOG_INFO() << "Moving to inspiratory position. Simulation will start in " << t1+t2+lungMovementTiming.sum() << " seconds.";
}

void Ur5LungSimulation::startInspirationSequence()
{
    //CX_LOG_INFO() << "Inspiration sequence called. Inspiration move starts in " << lungMovementTiming.sum() << " seconds.";
    connect(inspiration,SIGNAL(timeout()), this, SLOT(continueLungMove()));
    inspiration->start(lungMovementTiming.sum()*1000);
    //CX_LOG_INFO() << "Inspiration interval " << inspiration->interval();
}

void Ur5LungSimulation::startExpirationSequence()
{
    //CX_LOG_INFO() << "Expiration sequence called. Expiration move starts in " << lungMovementTiming.sum() << " seconds.";
    connect(expiration,SIGNAL(timeout()),this,SLOT(continueLungMove()));
    expiration->start(lungMovementTiming.sum()*1000);
    //CX_LOG_INFO() << "Expiration interval " << expiration->interval();
}

void Ur5LungSimulation::continueLungMove()
{
    //CX_LOG_INFO() << "ContinueLungMove called";
    mProgramQueue.erase(mProgramQueue.begin());

    if(mProgramQueue.empty())
    {
        inspiration->stop();
        expiration->stop();
        std::cout << "Empty programQueue" << std::endl;
    }
    else
    {
        //CX_LOG_INFO() << mProgramQueue[0].toStdString().substr(56,57);
        mUr5Robot->sendMessage(mProgramQueue[0]);
    }
}

void Ur5LungSimulation::stopLungMove()
{
    inspiration->stop();
    expiration->stop();

    disconnect(inspiration, SIGNAL(timeout()), this, SLOT(continueLungMove()));
    disconnect(expiration,SIGNAL(timeout()),this,SLOT(continueLungMove()));

    if(!mProgramQueue.empty())
        mProgramQueue.clear();

    CX_LOG_INFO() << "Lung simulation stopped";
}

} // cx


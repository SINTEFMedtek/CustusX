#include "cxUr5Robot.h"

namespace cx
{

Ur5Robot::Ur5Robot()
{
    moveInProgress = false;

    connect(&mRTMonitor,&Ur5Connection::stateChanged,this,&Ur5Robot::updateCurrentState);
    connect(&mSecMonitor,&Ur5Connection::stateChanged,this,&Ur5Robot::updateCurrentState);
    connect(this,&Ur5Robot::atTarget,this,&Ur5Robot::atTargetSlot);

    mBlendRadius=0.001;
}

Ur5Robot::~Ur5Robot()
{
}

void Ur5Robot::atTargetSlot()
{
    this->nextMove();
}

void Ur5Robot::nextMove()
{
    if(moveInProgress && !mProgramEncoder.jointPositionQueue.empty())
    {
        mProgramEncoder.jointPositionQueue.erase(mProgramEncoder.jointPositionQueue.begin());
        if(mProgramEncoder.jointPositionQueue.empty())
        {
            moveInProgress=false;
        }
        else
        {
        mTargetState.jointPosition = mProgramEncoder.jointPositionQueue[0];
        this->move("movej",mTargetState.jointPosition,moveAcceleration,moveVelocity);
        }
    }
    else if(moveInProgress && !mProgramEncoder.poseQueue.empty())
    {
        mProgramEncoder.poseQueue.erase(mProgramEncoder.poseQueue.begin());
        if(mProgramEncoder.poseQueue.empty())
        {
            moveInProgress=false;
        }
        else
        {
        mTargetState.cartAxis = mProgramEncoder.poseQueue[0].cartAxis; //+ mStartPosition.cartAxis;
        mTargetState.cartAngles = mStartPosition.cartAngles;
        this->move("movej",mTargetState,moveAcceleration,moveVelocity);
        }
    }
}

void Ur5Robot::updateCurrentState()
{
    this->setPreviousState(getCurrentState());

    Ur5State currentState;

    currentState.timeSinceStart=mRTMonitor.getCurrentState().timeSinceStart;
    currentState.cartAngles=mSecMonitor.getCurrentState().cartAngles;
    currentState.cartAxis=mSecMonitor.getCurrentState().cartAxis;
    currentState.force=mRTMonitor.getCurrentState().force;
    currentState.torque=mRTMonitor.getCurrentState().torque;
    currentState.jointPosition=mRTMonitor.getCurrentState().jointPosition;
    currentState.jointVelocity=mRTMonitor.getCurrentState().jointVelocity;
    currentState.tcpAngles=mRTMonitor.getCurrentState().tcpAngles;
    currentState.tcpAxis=mRTMonitor.getCurrentState().tcpAxis;
    currentState.baseMee = mSecMonitor.getCurrentState().baseMee;

    emit transform("RobotTracker",currentState.baseMee,currentState.timeSinceStart);

    this->setCurrentState(currentState);
    emit(stateUpdated());
    this->atTargetState();
}

Ur5State Ur5Robot::getCurrentState()
{
    return (this->mCurrentState);
}

Ur5State Ur5Robot::getPreviousState()
{
    return (this->mPreviousState);
}

void Ur5Robot::setCurrentState(Ur5State currentState)
{
    this->mCurrentState=currentState;
}

void Ur5Robot::setPreviousState(Ur5State previousState)
{
    this->mPreviousState=previousState;
}

void Ur5Robot::setAddress(QString address)
{
    IPaddress = address;
}

QString Ur5Robot::getAddress()
{
    return (this->IPaddress);
}

void Ur5Robot::connectToRobot(QString IPaddress)
{
    mRTMonitor.setAddress(IPaddress,rtPort);
    mSecMonitor.setAddress(IPaddress,secPort);

    connectToPort(rtPort);
    connectToPort(secPort);

    if(isConnectedToRobot())
        emit(connected());
}

void Ur5Robot::disconnectFromRobot()
{
    disconnectFromPort(rtPort);
    disconnectFromPort(secPort);

    if(!isConnectedToRobot())
        emit(disconnected());
}

void Ur5Robot::connectToPort(int port)
{
    if(port == 30003)
    {
        mRTMonitor.requestConnect();
    }
    else if(port == 30002)
    {
        mSecMonitor.requestConnect();
    }
}

void Ur5Robot::disconnectFromPort(int port)
{
    if(port == 30003)
    {
        mRTMonitor.requestDisconnect();
    }
    else if(port == 30002)
    {
        mSecMonitor.requestDisconnect();
    }
}

bool Ur5Robot::isConnectedToRobot()
{
    return (mRTMonitor.isConnectedToRobot() && mSecMonitor.isConnectedToRobot());
}

void Ur5Robot::shutdown()
{
    sendMessage(mMessageEncoder.powerdown());
    disconnectFromRobot();
    emit(shuttingdown());
}

void Ur5Robot::sendMessage(QString message)
{
    mSecMonitor.sendMessage(message);
}

void Ur5Robot::move(QString typeOfMovement, Ur5State targetState, double acc, double vel, double rad, double t)
{
    mTargetState = targetState;

    if(typeOfMovement=="movej")
        sendMessage(mMessageEncoder.movej(targetState,acc,vel,0));
    else if(typeOfMovement=="speedl")
        sendMessage(mMessageEncoder.speedl(targetState,acc,t));
    else if(typeOfMovement =="speedj")
        sendMessage(mMessageEncoder.speedj(targetState,acc,t));
}

void Ur5Robot::move(QString typeOfMovement, Eigen::RowVectorXd targetState, double acc, double vel, double t, double rad)
{
    mTargetState.jointPosition = targetState;

    if(typeOfMovement=="movej")
        sendMessage(mMessageEncoder.movej(targetState,acc,vel,t,rad));
    else if(typeOfMovement=="speedl")
        sendMessage(mMessageEncoder.speedl(targetState,acc,t));
    else if(typeOfMovement =="speedj")
        sendMessage(mMessageEncoder.speedj(targetState,acc,t));

}

void Ur5Robot::addToMoveQueue(Eigen::RowVectorXd target)
{
    mProgramEncoder.jointPositionQueue.push_back(target);
}

void Ur5Robot::addToProgramQueue(QString str)
{
    mProgramEncoder.programQueue.push_back(str);
}

void Ur5Robot::clearProgramQueue()
{
    mProgramEncoder.programQueue.clear();
}

void Ur5Robot::stopMove(QString typeOfStop, double acc)
{
    if(typeOfStop=="stopl")
        sendMessage(mMessageEncoder.stopl(acc));
    else if(typeOfStop=="stopj")
        sendMessage(mMessageEncoder.stopj(acc));
}

void Ur5Robot::openVTKfile(QString filename)
{
    mProgramEncoder.openVTKfile(filename);
}

void Ur5Robot::moveProgram(QString typeOfProgram,double acceleration,double velocity, double radius, double t)
{
    if(typeOfProgram == "movej")
    {
        mProgramEncoder.movejProgram(mProgramEncoder.poseQueue,acceleration,velocity,radius);
        mStartPosition = this->getCurrentState();
        Ur5State bah;
        bah.cartAxis = mProgramEncoder.poseQueue[0].cartAxis; //+ mStartPosition.cartAxis;
        bah.cartAngles = mStartPosition.cartAngles;
        this->move("movej",bah,acceleration,velocity);
        moveInProgress=true;
        moveAcceleration=acceleration;
        moveVelocity=velocity;
    }
    else if(typeOfProgram == "movej2")
    {
        mProgramEncoder.movejProgram(mProgramEncoder.jointPositionQueue,acceleration,velocity,radius);
        this->move("movej",mProgramEncoder.jointPositionQueue[0],acceleration,velocity);
        moveInProgress=true;
        moveAcceleration=acceleration;
        moveVelocity=velocity;
    }
    else
    {
        return;
    }
}

bool Ur5Robot::atTargetState()
{
    if((mCurrentState.jointPosition-mTargetState.jointPosition).length()<mBlendRadius)
    {
        emit atTarget();
        return true;
    }
    else if((mCurrentState.cartAxis-mTargetState.cartAxis).length()<mBlendRadius)
    {
        emit atTarget();
        return true;
    }
    return false;
}

void Ur5Robot::printMoveQueue()
{
    for(int i = 0; i<mProgramEncoder.jointPositionQueue.size(); i++)
        std::cout << mProgramEncoder.jointPositionQueue[i] << std::endl;
}

void Ur5Robot::printProgramQueue()
{
    for(int i = 0; i<mProgramEncoder.programQueue.size(); i++)
        std::cout << mProgramEncoder.programQueue[i].toStdString() << std::endl;
}

void Ur5Robot::setBlendRadius(double blendRadius)
{
    std::cout << "Blend radius set to " << blendRadius << std::endl;
    mBlendRadius=blendRadius;
}

std::vector<QString> Ur5Robot::getProgramQueue()
{
    return this->mProgramEncoder.programQueue;
}


} // cx


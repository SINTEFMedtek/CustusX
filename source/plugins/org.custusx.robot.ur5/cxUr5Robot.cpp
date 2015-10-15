#include "cxUr5Robot.h"

namespace cx
{

Ur5Robot::Ur5Robot()
{
    connect(&mRTMonitor,&Ur5Connection::stateChanged,this,&Ur5Robot::updateCurrentState);
    connect(&mSecMonitor,&Ur5Connection::stateChanged,this,&Ur5Robot::updateCurrentState);
}

Ur5Robot::~Ur5Robot()
{
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
    if(typeOfMovement=="movej")
        sendMessage(mMessageEncoder.movej(targetState,acc,vel,0));
    else if(typeOfMovement=="speedl")
        sendMessage(mMessageEncoder.speedl(targetState,acc,t));
    else if(typeOfMovement =="speedj")
        sendMessage(mMessageEncoder.speedj(targetState,acc,t));
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

void Ur5Robot::moveProgram(QString typeOfProgram,double acceleration,double velocity, double radius)
{
    mRTMonitor.requestDisconnect();
    if(typeOfProgram == "movej")
    {
        mProgramEncoder.movejProgram(mProgramEncoder.poseQueue,acceleration,velocity,radius);
    }
    else
    {
        return;
    }
    mSecMonitor.runProgramQueue(mProgramEncoder.programQueue,mProgramEncoder.poseQueue);
    mRTMonitor.requestConnect();
}


} // cx


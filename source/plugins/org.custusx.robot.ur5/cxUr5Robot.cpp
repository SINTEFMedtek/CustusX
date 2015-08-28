#include "cxUr5Robot.h"

namespace cx
{

Ur5Robot::Ur5Robot()
{
    connect(&mRTMonitor,SIGNAL(stateChanged()),this,SLOT(updateCurrentState()));
    connect(&mSecMonitor,SIGNAL(stateChanged()),this,SLOT(updateCurrentState()));
}

Ur5Robot::~Ur5Robot()
{
}

void Ur5Robot::updateCurrentState()
{
    mPreviousState=mCurrentState;
    this->mCurrentState.timeSinceStart=this->mRTMonitor.mCurrentState.timeSinceStart;
    this->mCurrentState.cartAngles=this->mSecMonitor.mCurrentState.cartAngles;
    this->mCurrentState.cartAxis=this->mSecMonitor.mCurrentState.cartAxis;
    this->mCurrentState.force=this->mRTMonitor.mCurrentState.force;
    this->mCurrentState.torque=this->mRTMonitor.mCurrentState.torque;
    this->mCurrentState.jointAngles=this->mRTMonitor.mCurrentState.jointAngles;
    this->mCurrentState.jointAngleVelocity=this->mRTMonitor.mCurrentState.jointAngleVelocity;
    this->mCurrentState.jointAxis=this->mRTMonitor.mCurrentState.jointAxis;
    this->mCurrentState.jointAxisVelocity=this->mRTMonitor.mCurrentState.jointAxisVelocity;
    this->mCurrentState.tcpAngles=this->mRTMonitor.mCurrentState.tcpAngles;
    this->mCurrentState.tcpAxis=this->mRTMonitor.mCurrentState.tcpAxis;
    emit(stateUpdated());
}

void Ur5Robot::setAddress(QString address)
{
    IPaddress = address;
}

void Ur5Robot::connectToRobot(int port)
{
    mRTMonitor.setAddress(IPaddress,rtPort);
    mSecMonitor.setAddress(IPaddress,secPort);
    if(port == 30003)
    {
        mRTMonitor.requestConnect();
    }
    else if(port == 30002)
    {
        mSecMonitor.requestConnect();
    }
    else
    {
        mRTMonitor.requestConnect();
        mSecMonitor.requestConnect();
    }
}

void Ur5Robot::disconnectFromRobot(int port)
{
    if(port == 30003)
    {
        mRTMonitor.requestDisconnect();
    }
    else if(port == 30002)
    {
        mSecMonitor.requestDisconnect();
    }
    else
    {
        mRTMonitor.requestDisconnect();
        mSecMonitor.requestDisconnect();
    }
}

bool Ur5Robot::isConnectedToRobot()
{
    return (mRTMonitor.isConnectedToRobot() && mSecMonitor.isConnectedToRobot());
}

void Ur5Robot::initializeWorkspace(double threshold,Ur5State state,bool currentPos)
{
    mRTMonitor.requestDisconnect();
    mSecMonitor.initializeWorkspace(threshold,state,currentPos);
    mRTMonitor.requestConnect();
}

void Ur5Robot::shutdown()
{
    sendMessage(mMessageEncoder.powerdown());
    disconnectFromRobot();
}

void Ur5Robot::sendMessage(QString message)
{
    mSecMonitor.sendMessage(message);
}

void Ur5Robot::move(QString typeOfMovement, Ur5State targetState, double acc, double vel, double rad, double t)
{
    if(typeOfMovement=="movej")
        sendMessage(mMessageEncoder.movej(targetState,acc,vel,0));
}

void Ur5Robot::openVTKfile(QString filename)
{
    mProgramEncoder.openVTKfile(filename);
}

void Ur5Robot::moveProgram(double acceleration,double velocity, double radius)
{
    mRTMonitor.requestDisconnect();
    mProgramEncoder.movejProgram(mProgramEncoder.poseQueue,acceleration,velocity,radius);
    mSecMonitor.runProgramQueue(mProgramEncoder.programQueue,mProgramEncoder.poseQueue);
    mRTMonitor.requestConnect();
}



} // cx


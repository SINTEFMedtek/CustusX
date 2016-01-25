#include "cxUr5Robot.h"
#include "cxLogger.h"

namespace cx
{

Ur5Robot::Ur5Robot():
    moveInProgress(false),
    velocityMoveInProgress(false),
    mBlendRadius(0.001),
    rtPort(30003),
    secPort(30002),
    motionSpace(Transform3D::Identity())
{
    connect(&mRTMonitor,&Ur5Connection::stateChanged,this,&Ur5Robot::updateCurrentState);
    connect(&mSecMonitor,&Ur5Connection::stateChanged,this,&Ur5Robot::updateCurrentState);
    connect(this,&Ur5Robot::atTarget,this,&Ur5Robot::atTargetSlot);
    connect(this,&Ur5Robot::startLogging,this,&Ur5Robot::startLoggingSlot);
    connect(this,&Ur5Robot::stopLogging,this,&Ur5Robot::stopLoggingSlot);
    connect(this,&Ur5Robot::moveToInitialPosition,this,&Ur5Robot::moveToInitialPositionSlot);

    this->mCurrentState.jointConfiguration << 0,-1.57075,0,-1.57075,0,0;
    this->mCurrentState.bMee = mKinematics.forward(mCurrentState.jointConfiguration);

    emit(stateUpdated());
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
            mTargetState.jointConfiguration = mProgramEncoder.jointPositionQueue[0];
            this->move("movej",mTargetState.jointConfiguration,moveAcceleration,moveVelocity);
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
    else if(velocityMoveInProgress && !mProgramEncoder.poseQueue.empty())
    {
        mProgramEncoder.poseQueue.erase(mProgramEncoder.poseQueue.begin());
        if(mProgramEncoder.poseQueue.empty())
        {
            velocityMoveInProgress=false;
            this->move("stopj",mTargetState,moveAcceleration,moveVelocity);
        }
        else
        {
            if(mProgramEncoder.poseQueue.size()>1)
            {
                mTargetState.cartAxis = mProgramEncoder.poseQueue[0].cartAxis;
                mTargetState.cartAngles = mStartPosition.cartAngles;
                Vector3D tangent = mProgramEncoder.poseQueue[1].cartAxis-mCurrentState.cartAxis;
                Eigen::RowVectorXd velocityEndEffector(6);
                velocityEndEffector << moveVelocity*tangent(0)/tangent.norm(),moveVelocity*tangent(1)/tangent.norm(),moveVelocity*tangent(2)/tangent.norm(),0,0,0;
                mTargetState.jointVelocity = mCurrentState.jacobian.inverse()*velocityEndEffector.transpose();
                this->move("speedj",mTargetState,moveAcceleration,moveVelocity,0,20);
            }
        }
    }
}

void Ur5Robot::updateCurrentState()
{
    this->setPreviousState(getCurrentState());

    Ur5State currentState;

    currentState.timeSinceStart=mRTMonitor.getCurrentState().timeSinceStart;
    currentState.jointConfiguration=mRTMonitor.getCurrentState().jointConfiguration;
    currentState.jointVelocity=mRTMonitor.getCurrentState().jointVelocity;
    currentState.bMee = mKinematics.forward(currentState.jointConfiguration);
    currentState.cartAxis= mKinematics.T2transl(currentState.bMee);
    currentState.cartAngles = mKinematics.T2rangles(currentState.bMee);
    currentState.jacobian = mKinematics.jacobian(currentState.jointConfiguration);
    currentState.operationalVelocity = currentState.jacobian*currentState.jointVelocity.transpose();

    Transform3D trackingMatrix = Transform3D(currentState.bMee);
    trackingMatrix.translation() = trackingMatrix.translation()*1000;

    emit transform("RobotTracker",trackingMatrix,currentState.timeSinceStart);

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

void Ur5Robot::move(QString typeOfMovement, Ur5State targetState, double acc, double vel, double t, double rad)
{
    mTargetState = targetState;

    if(typeOfMovement=="movej")
        sendMessage(mMessageEncoder.movej(targetState,acc,vel,0));
    else if(typeOfMovement=="speedl")
        sendMessage(mMessageEncoder.speedl(targetState,acc,t));
    else if(typeOfMovement =="speedj")
        sendMessage(mMessageEncoder.speedj(targetState,acc,t));
    else if(typeOfMovement =="stopj")
        sendMessage(mMessageEncoder.stopj(acc));
}

void Ur5Robot::move(QString typeOfMovement, Eigen::RowVectorXd targetState, double acc, double vel, double t, double rad)
{
    mTargetState.jointConfiguration = targetState;

    if(typeOfMovement=="movej")
        sendMessage(mMessageEncoder.movej(targetState,acc,vel,t,rad));
    else if(typeOfMovement=="speedl")
        sendMessage(mMessageEncoder.speedl(targetState,acc,t));
    else if(typeOfMovement =="speedj")
        sendMessage(mMessageEncoder.speedj(targetState,acc,t));
    else if(typeOfMovement =="stopj")
        sendMessage(mMessageEncoder.stopj(acc));

}

void Ur5Robot::move(Ur5MovementInfo movementInfo)
{
    mTargetState.jointConfiguration = movementInfo.targetJointConfiguration;

    if(movementInfo.typeOfMovement==Ur5MovementInfo::movementType::movej)
        sendMessage(mMessageEncoder.movej(movementInfo));
    if(movementInfo.typeOfMovement==Ur5MovementInfo::movementType::speedj)
        sendMessage(mMessageEncoder.speedj(movementInfo));
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
    if(!mProgramEncoder.programQueue.empty())
    {
        mProgramEncoder.programQueue.clear();
        mProgramEncoder.poseQueue.clear();
    }
    if(!mProgramEncoder.poseQueue.empty())
    {
        mProgramEncoder.clearQueues();
    }
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
        Ur5State initState;
        initState.cartAxis = mProgramEncoder.poseQueue[0].cartAxis; //+ mStartPosition.cartAxis;
        initState.cartAngles = mStartPosition.cartAngles;
        this->move("movej",initState,acceleration,velocity);
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
    else if(typeOfProgram == "speedj")
    {
        mStartPosition = this->getCurrentState();
        Ur5State initState;
        initState.cartAxis = mProgramEncoder.poseQueue[0].cartAxis;
        initState.cartAngles = mStartPosition.cartAngles;
        this->move("movej",initState,acceleration,velocity);
        moveAcceleration=acceleration;
        moveVelocity=velocity;
        velocityMoveInProgress=true;
    }
    else
    {
        return;
    }
}

bool Ur5Robot::atTargetState()
{
    if((mCurrentState.jointConfiguration-mTargetState.jointConfiguration).length()<mBlendRadius)
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

bool Ur5Robot::isValidWorkspace()
{
    return(abs(this->getCurrentState().jointConfiguration.maxCoeff())<=2*3.15);
}

bool Ur5Robot::isValidWorkspace(Eigen::RowVectorXd jointPosition)
{
    return(abs(jointPosition.maxCoeff())<=2*3.15);
}

void Ur5Robot::startLoggingSlot()
{
    connect(this,&Ur5Robot::stateUpdated,this,&Ur5Robot::dataLogger);
    CX_LOG_INFO() << "Logging started";
}

void Ur5Robot::stopLoggingSlot()
{
    disconnect(this,&Ur5Robot::stateUpdated,this,&Ur5Robot::dataLogger);
    CX_LOG_INFO() << "Logging stopped";
}

void Ur5Robot::dataLogger()
{
     CX_LOG_CHANNEL_INFO("jointConfiguration") << mCurrentState.jointConfiguration;
     CX_LOG_CHANNEL_INFO("jointVelocitites") << mCurrentState.jointVelocity;
     CX_LOG_CHANNEL_INFO("operationalPosition") << mCurrentState.cartAxis;
     CX_LOG_CHANNEL_INFO("operationalVelocity") << mCurrentState.operationalVelocity;
}

void Ur5Robot::moveToInitialPositionSlot(double acceleration, double velocity)
{
    mStartPosition = this->getCurrentState();
    Ur5State initState;
    initState.cartAxis = mProgramEncoder.poseQueue[0].cartAxis;
    initState.cartAngles = mStartPosition.cartAngles;
    this->move("movej",initState,acceleration,velocity);
}

} // cx


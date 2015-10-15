#ifndef UR5ROBOT_H
#define UR5ROBOT_H

#include "org_custusx_robot_ur5_Export.h"

#include "cxUr5Connection.h"
#include "cxUr5MessageEncoder.h"
#include "cxUr5State.h"

namespace cx
{
typedef boost::shared_ptr<class Ur5Robot> Ur5RobotPtr;

class org_custusx_robot_ur5_EXPORT Ur5Robot : public QObject
{
    Q_OBJECT

    friend class Ur5LungSimulation;

public:
    Ur5Robot();
    ~Ur5Robot();

    bool isConnectedToRobot();

    Ur5State getCurrentState();
    Ur5State getPreviousState();
    void setCurrentState(Ur5State currentState);
    void setPreviousState(Ur5State previousState);

    QString getAddress();
    void setAddress(QString IPaddress);

    void move(QString typeOfMovement, Ur5State targetState, double acc, double vel, double rad = 0, double t = 0);
    void move(QString typeOfMovement, Eigen::RowVectorXd targetState, double acc, double vel, double rad = 0, double t = 0);
    void stopMove(QString typeOfStop,double acc);

    void addToMoveQueue(Eigen::RowVectorXd target);

    void addToProgramQueue(QString string);
    void clearProgramQueue();

    void runProgramQueue(std::vector<QString> programQueue);

    void printMoveQueue();
    void printProgramQueue();

    void openVTKfile(QString filename);
    void moveProgram(QString typeOfProgram,double acceleration, double velocity, double radius, double t = 0);

    void setBlendRadius(double blendRadius);

    std::vector<QString> getProgramQueue();

public slots:
    void connectToRobot(QString IPaddress);
    void disconnectFromRobot();
    void updateCurrentState();
    void shutdown();


private slots:
    void nextMove();
    void atTargetSlot();

signals:
    void transform(QString devicename, Transform3D transform, double timestamp);
    void stateUpdated();
    void connected();
    void disconnected();

    void startTracking();
    void stopTracking();

    void shuttingdown();
    void atTarget();

private:
    int rtPort = 30003;
    int secPort = 30002;
    QString IPaddress;
    Ur5Connection mSecMonitor,mRTMonitor;
    Ur5ProgramEncoder mProgramEncoder;
    Ur5MessageEncoder mMessageEncoder;
    Ur5State mCurrentState, mPreviousState, mTargetState, mStartPosition;

    double moveVelocity,moveAcceleration;

    bool atTargetState();

    void sendMessage(QString message);
    void connectToPort(int port);
    void disconnectFromPort(int port);
    bool moveInProgress;

    double mBlendRadius;
};

} // cx

#endif // UR5ROBOT_H

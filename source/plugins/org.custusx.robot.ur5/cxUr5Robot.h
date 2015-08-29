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
    void stopMove(QString typeOfStop,double acc);

    void openVTKfile(QString filename);
    void moveProgram(QString typeOfProgram,double acceleration, double velocity, double radius);


public slots:
    void connectToRobot(QString IPaddress);
    void disconnectFromRobot();

    void updateCurrentState();
    void initializeWorkspace(double threshold,Ur5State state,bool currentPos);
    void shutdown();

private slots:
    void connectToPort(int port);
    void disconnectFromPort(int port);


signals:
    void stateUpdated();

private:
    int rtPort = 30003;
    int secPort = 30002;
    QString IPaddress;
    Ur5Connection mSecMonitor,mRTMonitor;
    Ur5ProgramEncoder mProgramEncoder;
    Ur5MessageEncoder mMessageEncoder;
    Ur5State mCurrentState, mPreviousState;

    void sendMessage(QString message);

};

} // cx

#endif // UR5ROBOT_H

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

    Ur5Connection mSecMonitor,mRTMonitor;
    Ur5ProgramEncoder mProgramEncoder;
    Ur5State mCurrentState, mPreviousState;
    Ur5MessageEncoder mMessageEncoder;

    void setAddress(QString IPaddress);

    bool isConnectedToRobot();
    void sendMessage(QString message);
    void move(QString typeOfMovement);
    void openVTKfile(QString filename);
    void moveProgram(std::vector<Ur5State> poseQueue,double acceleration, double velocity, double radius);


public slots:
    void updateCurrentState();
    void connectToRobot(int port = 0);
    void disconnectFromRobot(int port = 0);
    void initializeWorkspace(double threshold,Ur5State state,bool currentPos);
    void shutdown();

private:
    int rtPort = 30003;
    int secPort = 30002;
    QString IPaddress;


};

} // cx

#endif // UR5ROBOT_H

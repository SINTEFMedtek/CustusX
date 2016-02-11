#ifndef UR5ROBOT_H
#define UR5ROBOT_H

#include "org_custusx_robot_ur5_Export.h"

#include "cxUr5Connection.h"
#include "cxUr5MessageEncoder.h"
#include "cxUr5State.h"
#include "cxUr5Kinematics.h"

namespace cx
{
/**
 * Robotobject umbrella class
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \author Andreas Østvik
 *
 */

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

    void move(QString typeOfMovement, Ur5State targetState, double acc, double vel, double t = 0, double rad = 0);
    void move(QString typeOfMovement, Eigen::RowVectorXd targetState, double acc, double vel, double t = 0, double rad = 0);
    void move(Ur5MovementInfo movementInformation);

    void stopMove(QString typeOfStop,double acc);

    void addToProgramQueue(QString string);
    void clearProgramQueue();

    void openVTKfile(QString filename);
    void moveProgram(QString typeOfProgram,double acceleration, double velocity, double radius, double t = 0);

    void setBlendRadius(double blendRadius);

    void sendMessage(QString message);

    bool isValidWorkspace();
    bool isValidWorkspace(Eigen::RowVectorXd jointPosition);

    void set_eMt(Transform3D eMt);
    Transform3D get_eMt();

public slots:
    void connectToRobot(QString IPaddress);
    void disconnectFromRobot();
    void shutdown();

private slots:
    void nextMove();
    void updateCurrentState();

signals:
    void transform(QString devicename, Transform3D transform, double timestamp);
    void stateUpdated();
    void connected();
    void disconnected();

    void startTracking();
    void stopTracking();
    void addRobotVisualizationLinks();
    void removeRobotVisualizationLinks();

    void shuttingdown();
    void atTarget();

    void eMtChanged(Transform3D eMt);

private:
    void setCurrentState(Ur5State currentState);
    void setPreviousState(Ur5State previousState);

    QString getAddress();
    void setAddress(QString IPaddress);

    void addToMoveQueue(Eigen::RowVectorXd target);

    void runProgramQueue(std::vector<QString> programQueue);

    std::vector<QString> getProgramQueue();

    int rtPort, secPort;
    QString IPaddress;
    Ur5Connection mSecMonitor,mRTMonitor;
    Ur5ProgramEncoder mProgramEncoder;
    Ur5MessageEncoder mMessageEncoder;
    Ur5State mCurrentState, mPreviousState, mTargetState, mStartPosition;

    double moveVelocity,moveAcceleration;

    bool atTargetState();

    void connectToPort(int port);
    void disconnectFromPort(int port);
    bool moveInProgress;
    bool velocityMoveInProgress;

    double mBlendRadius;

    Transform3D motionSpace, eMt;

    void set_tcp(Transform3D eMt);
};

} // cx

#endif // UR5ROBOT_H

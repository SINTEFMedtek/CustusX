#ifndef UR5LUNGSIMULATION_H
#define UR5LUNGSIMULATION_H


#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QObject>

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5LungSimulation : public QObject
{
    Q_OBJECT

public:
    Ur5LungSimulation(Ur5RobotPtr Ur5Robot);
    Ur5LungSimulation();
    ~Ur5LungSimulation();

signals:
    void lungMovement(MovementQueue mq, double inspiratoryPauseTime, double expiratoryPauseTime);
    void stopLungMovement();

private slots:
    void lungMovementSlot(MovementQueue mq, double inspiratoryPauseTime, double expiratoryPauseTime);
    void continueLungMove();
    void stopLungMove();
    void startInspirationSequence();
    void startExpirationSequence();

private:
    Ur5RobotPtr mUr5Robot;
    QTimer *inspiration, *expiration, *inspirationTiming, *expirationTiming;
    Eigen::RowVectorXd lungMovementTiming;

    MovementQueue mMovementQueue;

    void stopTimers();
};

} // cx

#endif // UR5LUNGSIMULATION_H

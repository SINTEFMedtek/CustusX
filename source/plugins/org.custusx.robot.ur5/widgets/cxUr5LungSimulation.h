#ifndef UR5LUNGSIMULATION_H
#define UR5LUNGSIMULATION_H


#include "org_custusx_robot_ur5_Export.h"

#include <QWidget>

#include "cxUr5Robot.h"

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5LungSimulationTab: public QWidget
{
    Q_OBJECT
public:
    Ur5LungSimulationTab(Ur5RobotPtr Ur5Robot,QWidget *parent = 0);
    ~Ur5LungSimulationTab();

private:
    Ur5RobotPtr mUr5Robot;
};



} // cx


#endif // UR5LUNGSIMULATION_H

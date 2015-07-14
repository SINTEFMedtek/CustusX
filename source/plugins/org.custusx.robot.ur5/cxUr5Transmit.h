#ifndef UR5TRANSMIT_H
#define UR5TRANSMIT_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"
#include <vtkPolyData.h>

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5Transmit
{
public:
    std::vector<Ur5State> poseQueue;
    std::vector<QString> programQueue;

    void addToPoseQueue(Ur5State pose);
    void printPoseQueue();

    void movejProgram(std::vector<Ur5State> poseQueue, double a, double v, double r);

    int openVTKfile(QString filename);
    void printVTKline(vtkPolyData* output);
    void addPath(vtkPolyData* output);

    QString movej(Ur5State pose,double a, double v, double r);
    QString movel(Ur5State pose,double a, double v);
    QString speedj(double* velocityField, double a, double t);

};


} // cx

#endif // UR5TRANSMIT_H

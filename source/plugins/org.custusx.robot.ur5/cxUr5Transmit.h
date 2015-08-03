#ifndef UR5TRANSMIT_H
#define UR5TRANSMIT_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"
#include <vtkPolyData.h>

namespace cx
{
/**
 * Struct that handles UR5 robot transmittion messages
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \author Andreas Østvik
 * \date 2015-07-10
 */

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
    QString speedj(Ur5State velocityField, QString a, QString t);
    QString speedl(Ur5State velocityField, QString a, QString t);
    QString stopj(QString a);
    QString stopl(QString a);
    QString set_tcp(Ur5State pose);

};


} // cx

#endif // UR5TRANSMIT_H

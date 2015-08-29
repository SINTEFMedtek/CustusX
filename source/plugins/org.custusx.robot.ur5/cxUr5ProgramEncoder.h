#ifndef UR5TRANSMIT_H
#define UR5TRANSMIT_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"
#include "cxUr5MessageEncoder.h"

#include <vtkPolyData.h>


namespace cx
{
/**
 * Class that handles UR5 robot program encoding
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \author Andreas Østvik
 * \date 2015-07-10
 */

class org_custusx_robot_ur5_EXPORT Ur5ProgramEncoder
{
public:
    std::vector<Ur5State> poseQueue;
    std::vector<QString> programQueue;

    void movejProgram(std::vector<Ur5State> poseQueue, double a, double v, double r);

    int openVTKfile(QString filename);
    void printVTKline(vtkPolyData* output);

private:
    void addPath(vtkPolyData* output);
    void addToPoseQueue(Ur5State pose);
    Ur5MessageEncoder mMessageEncoder;

};


} // cx

#endif // UR5TRANSMIT_H

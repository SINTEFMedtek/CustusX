#ifndef UR5TRANSMIT_H
#define UR5TRANSMIT_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"
#include "cxUr5MessageEncoder.h"

#include <vtkPolyData.h>
#include "cxDataReaderWriter.h"


namespace cx
{
/**
 * Class that handles UR5 robot program encoding
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \author Andreas Østvik
 *
 */

class org_custusx_robot_ur5_EXPORT Ur5ProgramEncoder
{
public:
    int openVTKfile(QString filename);
    void clearQueues();

    std::vector<Ur5State> poseQueue;
    std::vector<QString> programQueue;
    std::vector<Eigen::RowVectorXd> jointPositionQueue;




private:
    void addPath(vtkPolyData* output);
    void addToPoseQueue(Ur5State pose);
    Ur5MessageEncoder mMessageEncoder;

    vtkPolyDataPtr getPolyDataFromFile(QString inputFilename) const;
    std::vector<Transform3D> getTransformationsFromPolyData(vtkPolyData* output);

    std::vector<Ur5MovementInfo> mMovementQueue;
};


} // cx

#endif // UR5TRANSMIT_H

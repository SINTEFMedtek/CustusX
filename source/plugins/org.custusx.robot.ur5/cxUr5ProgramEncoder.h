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
    int openVTKfile(QString filename); // Mål å fjærne
    void clearQueues(); // Mål å fjærne

    std::vector<Ur5State> poseQueue; // Mål å fjærne
    std::vector<QString> programQueue; // Mål å fjærne
    std::vector<Eigen::RowVectorXd> jointPositionQueue; // Mål å fjærne

    static MovementQueue createMovementQueueFromVTKFile(QString inputFilename);
    static MovementQueue createMovementQueueFromTransformations(std::vector<Transform3D> transforms);

    static MovementQueue addMovementSettings(MovementQueue mq, double acc = 0, double vel = 0,
                              double time = 0, double radius = 0);
    static MovementQueue addTypeOfMovement(MovementQueue mq, Ur5MovementInfo::movementType mtype);
    static MovementQueue addMotionReference(MovementQueue mq, Transform3D prMb);
    static MovementQueue setConstantOrientation(MovementQueue mq, Transform3D mat);

private:
    void addPath(vtkPolyData* output); // Mål å fjærne
    void addToPoseQueue(Ur5State pose); // Mål å fjærne

    static vtkPolyDataPtr getPolyDataFromFile(QString inputFilename);
    static std::vector<Transform3D> getTransformationsFromPolyData(vtkPolyData* output);
    static Ur5MovementInfo createMovementInfoWithTransformation(Transform3D transform);
};


} // cx

#endif // UR5TRANSMIT_H

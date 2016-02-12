#include "cxUr5ProgramEncoder.h"

#include "cxLogger.h"
#include <vtkPolyDataReader.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <string>

#include "cxUr5State.h"


namespace cx
{

void Ur5ProgramEncoder::addToPoseQueue(Ur5State pose)
{
    poseQueue.push_back(pose);
}

int Ur5ProgramEncoder::openVTKfile(QString inputFilename)
{
    vtkSmartPointer<vtkGenericDataObjectReader> reader =
        vtkSmartPointer<vtkGenericDataObjectReader>::New();

    reader->SetFileName(inputFilename.toStdString().c_str());
    reader->Update();

    if(reader->IsFilePolyData())
      {
      std::cout << "Output is a polydata" << std::endl;
      vtkPolyData* output = reader->GetPolyDataOutput();
      addPath(output);
      }

    return EXIT_SUCCESS;
}

void Ur5ProgramEncoder::addPath(vtkPolyData* output)
{
    for(vtkIdType i=0; i<output->GetCell(0)->GetNumberOfPoints(); i++)
    {
        Ur5State pose;
        double p[3];
        output->GetPoint(output->GetCell(0)->GetPointId(i),p);
        pose.cartAxis = Vector3D(p[0],p[1],p[2]);
        pose.cartAngles = Vector3D(0,0,0);
        addToPoseQueue(pose);
    }
}

void Ur5ProgramEncoder::clearQueues()
{
    poseQueue.clear();
    programQueue.clear();
    CX_LOG_INFO() << poseQueue.size();
}

vtkPolyDataPtr Ur5ProgramEncoder::getPolyDataFromFile(QString inputFilename)
{
    vtkSmartPointer<vtkGenericDataObjectReader> reader =
        vtkSmartPointer<vtkGenericDataObjectReader>::New();

    reader->SetFileName(inputFilename.toStdString().c_str());
    reader->Update();

    if(reader->IsFilePolyData())
      {
      return reader->GetPolyDataOutput();
      }

    return vtkPolyDataPtr::New();
}

std::vector<Transform3D> Ur5ProgramEncoder::getTransformationsFromPolyData(vtkPolyData* output)
{
    std::vector<Transform3D> transformations;

    for(vtkIdType i=0; i<output->GetCell(0)->GetNumberOfPoints(); i++)
    {
        Transform3D pose = Transform3D::Identity();
        double p[3];
        output->GetPoint(output->GetCell(0)->GetPointId(i),p);
        pose.translation() = Vector3D(p[0],p[1],p[2]);

        transformations.push_back(pose);
    }

    return transformations;
}

Ur5MovementInfo Ur5ProgramEncoder::createMovementInfoWithTransformation(Transform3D transform)
{
    Ur5MovementInfo movementInfo;
    movementInfo.target_xMe = transform;
    return movementInfo;
}

MovementQueue Ur5ProgramEncoder::addMovementSettings(MovementQueue mq, double acceleration, double velocity, double time, double radius)
{
    for(int i=0; i<(mq.size()); i++)
    {
        mq.at(i).acceleration = acceleration;
        mq.at(i).velocity = velocity;
        mq.at(i).time = time;
        mq.at(i).radius = radius;
    }
    return mq;
}

MovementQueue Ur5ProgramEncoder::addTypeOfMovement(MovementQueue mq, Ur5MovementInfo::movementType typeOfMovement)
{
    for(int i=0; i<(mq.size()); i++)
    {
        mq.at(i).typeOfMovement = typeOfMovement;
    }
    return mq;
}

MovementQueue Ur5ProgramEncoder::addMotionReference(MovementQueue mq, Transform3D prMb)
{
    for(int i=0; i<(mq.size()); i++)
    {
        mq.at(i).motionReference = prMb;
    }
    return mq;
}

MovementQueue Ur5ProgramEncoder::createMovementQueueFromTransformations(std::vector<Transform3D> transformations)
{
    std::vector<Ur5MovementInfo> movementQueue(transformations.size());

    for(int i=0; i<transformations.size(); i++)
    {
        movementQueue.at(i).target_xMe = transformations.at(i);
    }

    return movementQueue;
}

MovementQueue Ur5ProgramEncoder::createMovementQueueFromVTKFile(QString inputFilename)
{
    std::vector<Ur5MovementInfo> movementQueue;

    std::vector<Transform3D> transformations
            = getTransformationsFromPolyData(getPolyDataFromFile(inputFilename));

    for(int i=0; i<transformations.size(); i++)
    {
        movementQueue.push_back(createMovementInfoWithTransformation(transformations.at(i)));
    }

    return movementQueue;
}

MovementQueue Ur5ProgramEncoder::setConstantOrientation(MovementQueue mq, Transform3D mat)
{
    for(int i=0; i<(mq.size()); i++)
    {
        mq.at(i).target_xMe.linear() = mat.linear();
    }
    return mq;
}

} //cx










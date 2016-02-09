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
        pose.cartAxis = Vector3D(p[0],p[1],p[2])/1000;
        pose.cartAngles = Vector3D(0,0,0);
        addToPoseQueue(pose);
    }
}

void Ur5ProgramEncoder::clearQueues()
{
    poseQueue.clear();
    programQueue.clear();
    mMovementQueue.clear();
    CX_LOG_INFO() << poseQueue.size();
}

vtkPolyDataPtr Ur5ProgramEncoder::getPolyDataFromFile(QString inputFilename) const
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

void Ur5ProgramEncoder::createMovementQueueFromVTKFile(QString inputFilename)
{
    std::vector<Ur5MovementInfo> movementQueue;

    std::vector<Transform3D> transformations
            = this->getTransformationsFromPolyData(this->getPolyDataFromFile(inputFilename));

    for(int i=0; i<transformations.size(); i++)
    {
        movementQueue.push_back(createMovementInfoWithTransformation(transformations.at(i)));
    }

    this->mMovementQueue = movementQueue;
}

void Ur5ProgramEncoder::createMovementQueueFromTransformations(std::vector<Transform3D> transformations)
{
    std::vector<Ur5MovementInfo> movementQueue(transformations.size());

    for(int i=0; i<transformations.size(); i++)
    {
        movementQueue.at(i).target_xMe = transformations.at(i);
    }

    this->mMovementQueue = movementQueue;
}

Ur5MovementInfo Ur5ProgramEncoder::createMovementInfoWithTransformation(Transform3D transform)
{
    Ur5MovementInfo movementInfo;
    movementInfo.target_xMe = transform;
    return movementInfo;
}

std::vector<Ur5MovementInfo> Ur5ProgramEncoder::getMovementQueue()
{
    return mMovementQueue;
}

void Ur5ProgramEncoder::setMovementSettings(double acceleration, double velocity, double time, double radius)
{
    for(int i=0; i<this->mMovementQueue.size(); i++)
    {
        mMovementQueue.at(i).acceleration = acceleration;
        mMovementQueue.at(i).velocity = velocity;
        mMovementQueue.at(i).time = time;
        mMovementQueue.at(i).radius = radius;
    }
}

void Ur5ProgramEncoder::setTypeOfMovement(Ur5MovementInfo::movementType typeOfMovement)
{
    for(int i=0; i<(this->mMovementQueue.size()); i++)
    {
        mMovementQueue.at(i).typeOfMovement = typeOfMovement;
    }
}

void Ur5ProgramEncoder::setMotionReference(Transform3D prMb)
{
    for(int i=0; i<(this->mMovementQueue.size()); i++)
    {
        mMovementQueue.at(i).motionReference = prMb;
    }
}

} //cx










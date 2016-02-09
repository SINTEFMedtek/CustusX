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

} //cx










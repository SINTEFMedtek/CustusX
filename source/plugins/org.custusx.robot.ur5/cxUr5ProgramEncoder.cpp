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
    movementQueue.clear();
    CX_LOG_INFO() << poseQueue.size();
}

} //cx










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

void Ur5ProgramEncoder::movejProgram(std::vector<Ur5State> poseQueue,double a, double v, double r)
{
    for(int i=0;i<poseQueue.size();i++)
    {
        programQueue.push_back(mMessageEncoder.movej(poseQueue[i],a,v,r));
    }
}

void Ur5ProgramEncoder::movejProgram(std::vector<Eigen::RowVectorXd> jointPositionQueue,double a, double v, double t, double r)
{
    for(int i=0;i<jointPositionQueue.size();i++)
    {
        programQueue.push_back(mMessageEncoder.movej(jointPositionQueue[i],a,v,t,r));
    }
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
      std::cout << output->GetNumberOfPoints() << std::endl;
      addPath(output);
      }

    return EXIT_SUCCESS;
}

void Ur5ProgramEncoder::printVTKline(vtkPolyData* output)
{
    std::cout << "line has " << output->GetLines()->GetNumberOfConnectivityEntries() << " points" << std::endl;

    for(vtkIdType i=0; i<output->GetCell(0)->GetNumberOfPoints(); i++)
    {
        double p[3];
        output->GetPoint(output->GetCell(0)->GetPointId(i),p);
        std::cout << "Point " << output->GetCell(0)->GetPointId(i) << " : (" << p[0] << " " << p[1] << " " << p[2] << ")" << std::endl;
    }
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










#include "cxUr5Transmit.h"

#include <vtkGenericDataObjectReader.h>
#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <string>


namespace cx
{

void Ur5Transmit::addToPoseQueue(Ur5State pose)
{
    poseQueue.push_back(pose);
}

void Ur5Transmit::printPoseQueue()
{
    for(int i=0; i<poseQueue.size(); i++)
    {
        std::cout << poseQueue[i].cartAxis*1000 << std::endl;
        std::cout << poseQueue[i].cartAngles << "\n" << std::endl;
    }

}

void Ur5Transmit::movejProgram(std::vector<Ur5State> poseQueue,double a, double v, double r)
{
    for(int i=0;i<poseQueue.size();i++)
    {
        programQueue.push_back(movej(poseQueue[i],a,v,r));
    }
}



QString Ur5Transmit::movej(Ur5State p,double a, double v,double r)
{
    return QString("movej(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8,r=%9)")
            .arg(p.cartAxis(0)).arg(p.cartAxis(1)).arg(p.cartAxis(2)).arg(p.cartAngles(0))
            .arg(p.cartAngles(1)).arg(p.cartAngles(2)).arg(a).arg(v).arg(r);
}

QString Ur5Transmit::movel(Ur5State p,double a, double v)
{
    return QString("movel(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8)")
            .arg(p.cartAxis(0)).arg(p.cartAxis(1)).arg(p.cartAxis(2)).arg(p.cartAngles(0))
            .arg(p.cartAngles(1)).arg(p.cartAngles(2)).arg(a).arg(v);
}

QString Ur5Transmit::speedj(Ur5State p, QString a, QString t)
{
    return QString("speedj([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(p.jointAxisVelocity(0)).arg(p.jointAxisVelocity(1)).arg(p.jointAxisVelocity(2)).arg(p.jointAngleVelocity(0))
            .arg(p.jointAngleVelocity(1)).arg(p.jointAngleVelocity(2)).arg(a).arg(t);
}

QString Ur5Transmit::speedl(Ur5State p, QString a, QString t)
{
    return QString("speedl([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(p.jointAxisVelocity(0)).arg(p.jointAxisVelocity(1)).arg(p.jointAxisVelocity(2)).arg(p.jointAngleVelocity(0))
            .arg(p.jointAngleVelocity(1)).arg(p.jointAngleVelocity(2)).arg(a).arg(t);
}

QString Ur5Transmit::stopl(QString a)
{
    return QString("stopl(%1)").arg(a);
}

QString Ur5Transmit::stopj(QString a)
{
    return QString("stopj(%1)").arg(a);
}

QString Ur5Transmit::set_tcp(Ur5State p)
{
    return QString("set_tcp(p[%1,%2,%3,%4,%5,%6])")
            .arg(p.cartAxis(0)).arg(p.cartAxis(1)).arg(p.cartAxis(2)).arg(p.cartAngles(0))
            .arg(p.cartAngles(1)).arg(p.cartAngles(2));
}

int Ur5Transmit::openVTKfile(QString inputFilename)
{
    // Get all data from the file
    vtkSmartPointer<vtkGenericDataObjectReader> reader =
        vtkSmartPointer<vtkGenericDataObjectReader>::New();

    reader->SetFileName(inputFilename.toStdString().c_str());
    reader->Update();

    // All of the standard data types can be checked and obtained like this:
    if(reader->IsFilePolyData())
      {
      std::cout << "output is a polydata" << std::endl;
      vtkPolyData* output = reader->GetPolyDataOutput();

      //printVTKinfo(output);
      addPath(output);


      }


    return EXIT_SUCCESS;


}

void Ur5Transmit::printVTKline(vtkPolyData* output)
{
    std::cout << "line has " << output->GetLines()->GetNumberOfConnectivityEntries() << " points" << std::endl;

    for(vtkIdType i=0; i<output->GetCell(0)->GetNumberOfPoints(); i++)
    {
        double p[3];
        output->GetPoint(output->GetCell(0)->GetPointId(i),p);
        std::cout << "Point " << output->GetCell(0)->GetPointId(i) << " : (" << p[0] << " " << p[1] << " " << p[2] << ")" << std::endl;
    }
}

void Ur5Transmit::addPath(vtkPolyData* output)
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


} //cx










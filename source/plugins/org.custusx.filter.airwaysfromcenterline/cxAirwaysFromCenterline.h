#ifndef CXAIRWAYSFROMCENTERLINE_H
#define CXAIRWAYSFROMCENTERLINE_H

#include "cxMesh.h"
#include <QDomElement>

typedef vtkSmartPointer<class vtkImageStencil> vtkImageStencilPtr;
typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;
typedef vtkSmartPointer<class vtkTubeFilter> vtkTubeFilterPtr;
typedef vtkSmartPointer<class vtkCleanPolyData> vtkCleanPolyDataPtr;
typedef vtkSmartPointer<class vtkPolyDataToImageStencil> vtkPolyDataToImageStencilPtr;

namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;
typedef boost::shared_ptr<class BranchList> BranchListPtr;
typedef boost::shared_ptr<class Branch> BranchPtr;


class AirwaysFromCenterline
{
public:
    AirwaysFromCenterline();
    virtual ~AirwaysFromCenterline();
    Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline_r);
    void processCenterline(vtkPolyDataPtr centerline_r);
    vtkPolyDataPtr generateTubes();
    vtkPolyDataPtr addVTKPoints(std::vector< Eigen::Vector3d > positions);

private:
	Eigen::MatrixXd mCLpoints;
	BranchListPtr mBranchListPtr;
};

double findDistanceToLine(Eigen::MatrixXd point, Eigen::MatrixXd line);
double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

} /* namespace cx */

#endif // CXAIRWAYSFROMCENTERLINE_H

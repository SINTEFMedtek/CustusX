#ifndef CXAIRWAYSFROMCENTERLINE_H
#define CXAIRWAYSFROMCENTERLINE_H

#include "cxMesh.h"
#include <QDomElement>
#include "org_custusx_filter_airwaysfromcenterline_Export.h"

typedef vtkSmartPointer<class vtkImageStencil> vtkImageStencilPtr;
typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;
typedef vtkSmartPointer<class vtkTubeFilter> vtkTubeFilterPtr;
typedef vtkSmartPointer<class vtkCleanPolyData> vtkCleanPolyDataPtr;
typedef vtkSmartPointer<class vtkPolyDataToImageStencil> vtkPolyDataToImageStencilPtr;
typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;

namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;
typedef boost::shared_ptr<class BranchList> BranchListPtr;
typedef boost::shared_ptr<class Branch> BranchPtr;


class org_custusx_filter_airwaysfromcenterline_EXPORT AirwaysFromCenterline
{
public:
    AirwaysFromCenterline();
    virtual ~AirwaysFromCenterline();
    Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline_r);
    void processCenterline(vtkPolyDataPtr centerline_r);
    vtkPolyDataPtr generateTubes();
    void createEmptyImage();
    void addPolyDataToImage(vtkPolyDataAlgorithmPtr mesh);
    vtkTubeFilterPtr createTube(vtkLineSourcePtr lineSourcePtr, double radius);
    vtkSphereSourcePtr createSphere(double position[3], double radius);
    vtkPolyDataPtr addVTKPoints(std::vector< Eigen::Vector3d > positions);
    vtkPolyDataPtr getVTKPoints();

private:
	Eigen::MatrixXd mCLpoints;
	BranchListPtr mBranchListPtr;
    vtkImageDataPtr mResultImagePtr;
    double mOrigin[3];
    double mSpacing[3];
    int mDim[3];
};

} /* namespace cx */

#endif // CXAIRWAYSFROMCENTERLINE_H

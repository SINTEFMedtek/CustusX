#ifndef CXBRONCHOSCOPEPOSITIONPROJECTION_H
#define CXBRONCHOSCOPEPOSITIONPROJECTION_H

#include "cxMesh.h"
#include <QDomElement>


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class BronchoscopePositionProjection> BronchoscopePositionProjectionPtr;


class BronchoscopePositionProjection
{
public:
	BronchoscopePositionProjection();
    BronchoscopePositionProjection(vtkPolyDataPtr centerline, Transform3D prMd);
	virtual ~BronchoscopePositionProjection();
	void setCenterline(vtkPolyDataPtr centerline, Transform3D prMd);
	void createMaxDistanceToCenterlineOption(QDomElement root);
	DoubleDataAdapterXmlPtr getMaxDistanceToCenterlineOption();
    Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline, Transform3D prMd);
	Transform3D findClosestPoint(Transform3D prMt, float maxDistance);

private:
	Eigen::MatrixXd mCLpoints;
	DoubleDataAdapterXmlPtr mMaxDistanceToCenterline;
};

} /* namespace cx */

#endif // CXBRONCHOSCOPEPOSITIONPROJECTION_H

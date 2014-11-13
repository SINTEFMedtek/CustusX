

#include "cxBronchoscopePositionProjection.h"
#include <vtkPolyData.h>

namespace cx
{

BronchoscopePositionProjection::BronchoscopePositionProjection(vtkPolyDataPtr centerline, Transform3D rMd)
{
	mCLpoints = this->getCenterlinePositions(centerline, rMd);
}

BronchoscopePositionProjection::~BronchoscopePositionProjection()
{
}

Eigen::MatrixXd BronchoscopePositionProjection::getCenterlinePositions(vtkPolyDataPtr centerline, Transform3D rMd)
{

	int N = centerline->GetNumberOfPoints();
	Eigen::MatrixXd CLpoints(3,N);
	for(vtkIdType i = 0; i < N; i++)
		{
		double p[3];
		centerline->GetPoint(i,p);
		Eigen::Vector3d position;
		position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
		CLpoints.block(0 , i , 3 , 1) = rMd.coord(position);
		}
	return CLpoints;
}


Transform3D BronchoscopePositionProjection::findClosestPoint(Transform3D prMt, float maxDistance)
{

	Eigen::VectorXd toolPos  = prMt.matrix().topRightCorner(3 , 1);
	Eigen::MatrixXd::Index index;
	Transform3D newprMt = prMt;

		Eigen::VectorXd P(mCLpoints.cols());

		for (int i = 0; i < mCLpoints.cols(); i++)
		{
			float p0 = ( mCLpoints(0,i) - toolPos(0) );
			float p1 = ( mCLpoints(1,i) - toolPos(1) );
			float p2 = ( mCLpoints(2,i) - toolPos(2) );

			P(i) = sqrt( p0*p0 + p1*p1 + p2*p2 );
		}

		P.minCoeff(&index);

		if (maxDistance > P.minCoeff())
			newprMt.matrix().topRightCorner(3 , 1) = mCLpoints.col(index);

		//std::cout << "index: " << index << std::endl;

	return newprMt;
}




} /* namespace cx */

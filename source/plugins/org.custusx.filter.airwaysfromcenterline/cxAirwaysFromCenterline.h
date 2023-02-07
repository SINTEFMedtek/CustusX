/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXAIRWAYSFROMCENTERLINE_H
#define CXAIRWAYSFROMCENTERLINE_H

#include "cxMesh.h"
#include "cxForwardDeclarations.h"
#include <QDomElement>
#include "org_custusx_filter_airwaysfromcenterline_Export.h"

namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;

class org_custusx_filter_airwaysfromcenterline_EXPORT AirwaysFromCenterline
{
public:
	AirwaysFromCenterline();
	virtual ~AirwaysFromCenterline();
	void setTypeToBloodVessel(bool bloodVessel);
	Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline_r);
	void setBranches(BranchListPtr branches);
	void setSegmentedVolume(vtkImageDataPtr segmentedVolume, Transform3D rMd = Transform3D::Identity());
	void processCenterline(vtkPolyDataPtr centerline_r);
	BranchListPtr getBranchList();
	vtkImageDataPtr getFilteredSegmentedVolume();
	vtkPolyDataPtr generateTubes(double staticRadius = 0, bool mergeWithOriginalAirways = false);
	vtkImageDataPtr initializeEmptyAirwaysVolume();
	vtkImageDataPtr initializeAirwaysVolumeFromOriginalSegmentation();
	vtkImageDataPtr addSpheresAlongCenterlines(vtkImageDataPtr airwaysVolumePtr, double staticRadius = 0);
	vtkImageDataPtr addSphereToImage(vtkImageDataPtr airwaysVolumePtr, double position[3], double radius);
	vtkImageDataPtr removeIslandsFromImage(vtkImageDataPtr image);
	void smoothAllBranchesForVB();
	vtkPolyDataPtr addVTKPoints(std::vector< Eigen::Vector3d > positions);
	vtkPolyDataPtr getVTKPoints();

private:
	Eigen::MatrixXd mCLpoints;
	BranchListPtr mBranchListPtr;
	vtkImageDataPtr mOriginalSegmentedVolume;
	vtkImageDataPtr mFilteredSegmentedVolumePtr;
	double mOrigin[3];
	Vector3D mSpacing;
	double mBounds[6];
	Eigen::Array3i mDim;
	double mAirwaysVolumeBoundaryExtention;
	double mAirwaysVolumeBoundaryExtentionTracheaStart;
	double mAirwaysVolumeSpacing;
	bool mBloodVessel = false;
	bool mMergeWithOriginalAirways = false;
	Transform3D m_rMd = Transform3D::Identity();

};

std::pair<int, double> findDistanceToLine(Eigen::Vector3d point, Eigen::MatrixXd line);
double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

typedef boost::shared_ptr<AirwaysFromCenterline> AirwaysFromCenterlinePtr;

} /* namespace cx */

#endif // CXAIRWAYSFROMCENTERLINE_H

// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXORIENTATIONANNOTATION2DREP_H_
#define CXORIENTATIONANNOTATION2DREP_H_

#include "cxRepImpl.h"
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include <vector>

namespace cx
{

typedef boost::shared_ptr<class OrientationAnnotationSmartRep> OrientationAnnotationSmartRepPtr;
typedef vtkSmartPointer<class OrientationAnnotation> OrientationAnnotationPtr;

/** \brief A class that annotated 2D views with otientation information.
 * \ingroup cx_resource_visualization
 *
 * The slice proxy is used to find the orientation of a slice in space r,
 * which is assumed to be a valid DICOM space, and combinations of
 * the letters
 * 	Anterior - Posterior
 *  Left - Right
 *  Superior - Inferior
 * are used. Combinations of the letters are used for oblique angles.
 */
class OrientationAnnotationSmartRep: public RepImpl
{
Q_OBJECT
public:
	static OrientationAnnotationSmartRepPtr New(const QString& uid, const QString& name);
	virtual ~OrientationAnnotationSmartRep();
	virtual QString getType() const	{ return "vm::OrientationAnnotationSmartRep"; }

	void setSliceProxy(SliceProxyPtr slicer);
	/**The maximum angular deviation from a major axis
	 * for annotation to be displayed.
	 */
	void ThresholdAngle(double angle);
	double ThresholdAngle() const;

	void setVisible(bool visible);
private slots:
	void transformChangedSlot();
protected:
	OrientationAnnotationSmartRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	QString determineAnnotation(Vector3D planeDir_s, Transform3D rMs);
	void createAnnotation();

	double mAngle;
	SliceProxyPtr mSlicer;
	OrientationAnnotationPtr mOrientation;
	std::map<QString, Vector3D> mDCMDirections_r; ///< directions of DICOM labels APSILR
	std::vector<Vector3D> mPlaneDirections_s; ///< the four directions in the slice plane
};

}

#endif /*CXORIENTATIONANNOTATION2DREP_H_*/


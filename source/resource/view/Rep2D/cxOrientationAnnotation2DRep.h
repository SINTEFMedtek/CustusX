/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXORIENTATIONANNOTATION2DREP_H_
#define CXORIENTATIONANNOTATION2DREP_H_

#include "cxResourceVisualizationExport.h"

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
 * \ingroup cx_resource_view
 *
 * The slice proxy is used to find the orientation of a slice in space r,
 * which is assumed to be a valid DICOM space, and combinations of
 * the letters
 * 	Anterior - Posterior
 *  Left - Right
 *  Superior - Inferior
 * are used. Combinations of the letters are used for oblique angles.
 */
class cxResourceVisualization_EXPORT OrientationAnnotationSmartRep: public RepImpl
{
Q_OBJECT
public:
	static OrientationAnnotationSmartRepPtr New(const QString& uid="");
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
	OrientationAnnotationSmartRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

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


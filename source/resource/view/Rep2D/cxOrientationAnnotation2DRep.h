/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


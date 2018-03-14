/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXORIENTATIONANNOTATIONREP_H_
#define CXORIENTATIONANNOTATIONREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include <vtkCornerAnnotation.h>

namespace cx
{

class cxResourceVisualization_EXPORT OrientationAnnotation : public vtkCornerAnnotation
{
	vtkTypeMacro(OrientationAnnotation, vtkCornerAnnotation);
public:
	static OrientationAnnotation* New();
	OrientationAnnotation();
	~OrientationAnnotation();
	virtual void SetTextActorsPosition(int vsize[2]);
	virtual void SetTextActorsJustification();
};

typedef vtkSmartPointer<class OrientationAnnotation> OrientationAnnotationPtr;

/**\brief Display direction annotations in a 2D view.
 *
 * Based on the input PLANE_TYPE, decorate with the letters APSILR (Anterior,
 * Posterior, Superior, Inferior, Left, Right) in the four main directions
 * of the view. Oblique views are not decorated.
 *
 * Use cx::OrientationAnnotationRep instead if decoration of oblique views
 * also is needed.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 */
class cxResourceVisualization_EXPORT OrientationAnnotationRep : public RepImpl
{
	Q_OBJECT
public:
	static OrientationAnnotationRepPtr  New(PatientModelServicePtr dataManager, const QString& uid="");
	virtual ~OrientationAnnotationRep();
	virtual QString getType() const { return "vm::OrientationAnnotationRep"; };

	void setPlaneType( PLANE_TYPE type);
	void setVisible(bool visible);
	private slots:
	void clinicalApplicationChangedSlot();
protected:
	OrientationAnnotationRep(PatientModelServicePtr dataManager);
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

	void setPlaneTypeNeurology(PLANE_TYPE type);
	void setPlaneTypeRadiology(PLANE_TYPE type);
	void createAnnotation();

	OrientationAnnotationPtr mOrientation;
	PLANE_TYPE mPlane;
	PatientModelServicePtr mDataManager;

	QString mNorthAnnotation;
	QString mSouthAnnotation;
	QString mEastAnnotation;
	QString mWestAnnotation;
};


}

#endif /*CXORIENTATIONANNOTATIONREP_H_*/


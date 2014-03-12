// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef VMORIENTATIONANNOTATIONREP_H_
#define VMORIENTATIONANNOTATIONREP_H_

#include "sscRepImpl.h"
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include <vtkCornerAnnotation.h>

namespace cx
{
class DataManager;

class OrientationAnnotation : public vtkCornerAnnotation
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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 */
class OrientationAnnotationRep : public RepImpl
{
	Q_OBJECT
public:
	static OrientationAnnotationRepPtr  New(DataServicePtr dataManager, const QString& uid,const QString& name);
	virtual ~OrientationAnnotationRep();
	virtual QString getType() const { return "vm::OrientationAnnotationRep"; };

	void setPlaneType( PLANE_TYPE type);
	void setVisible(bool visible);
	private slots:
	void clinicalApplicationChangedSlot();
protected:
	OrientationAnnotationRep(DataServicePtr dataManager, const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

	void setPlaneTypeNeurology(PLANE_TYPE type);
	void setPlaneTypeRadiology(PLANE_TYPE type);
	void createAnnotation();

	OrientationAnnotationPtr mOrientation;
	PLANE_TYPE mPlane;
	DataServicePtr mDataManager;

	QString mNorthAnnotation;
	QString mSouthAnnotation;
	QString mEastAnnotation;
	QString mWestAnnotation;
};


}

#endif /*VMORIENTATIONANNOTATIONREP_H_*/


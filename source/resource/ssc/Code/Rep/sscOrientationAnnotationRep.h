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

namespace ssc
{

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
 * \ingroup sscRep
 * \ingroup sscRep2D
 */
class OrientationAnnotationRep : public ssc::RepImpl
{
	Q_OBJECT
public:
	static OrientationAnnotationRepPtr  New(const QString& uid,const QString& name);
	virtual ~OrientationAnnotationRep();
	virtual QString getType() const { return "vm::OrientationAnnotationRep"; };

	void setPlaneType( PLANE_TYPE type);
	void setVisible(bool visible);
	private slots:
	void clinicalApplicationChangedSlot();
protected:
	OrientationAnnotationRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

	void setPlaneTypeNeurology(PLANE_TYPE type);
	void setPlaneTypeRadiology(PLANE_TYPE type);
	void createAnnotation();

	OrientationAnnotationPtr mOrientation;
	PLANE_TYPE mPlane;

	QString mNorthAnnotation;
	QString mSouthAnnotation;
	QString mEastAnnotation;
	QString mWestAnnotation;
};


}

#endif /*VMORIENTATIONANNOTATIONREP_H_*/


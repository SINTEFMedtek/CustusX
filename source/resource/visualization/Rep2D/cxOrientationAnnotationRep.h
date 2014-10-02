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
class DataManager;

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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 */
class cxResourceVisualization_EXPORT OrientationAnnotationRep : public RepImpl
{
	Q_OBJECT
public:
	static OrientationAnnotationRepPtr  New(DataServicePtr dataManager, const QString& uid="");
	virtual ~OrientationAnnotationRep();
	virtual QString getType() const { return "vm::OrientationAnnotationRep"; };

	void setPlaneType( PLANE_TYPE type);
	void setVisible(bool visible);
	private slots:
	void clinicalApplicationChangedSlot();
protected:
	OrientationAnnotationRep(DataServicePtr dataManager);
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

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

#endif /*CXORIENTATIONANNOTATIONREP_H_*/


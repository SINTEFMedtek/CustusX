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
#ifndef CXORIENTATIONANNOTATION3DREP_H_
#define CXORIENTATIONANNOTATION3DREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"

typedef vtkSmartPointer<class vtkOrientationMarkerWidget> vtkOrientationMarkerWidgetPtr;
typedef vtkSmartPointer<class vtkAnnotatedCubeActor> vtkAnnotatedCubeActorPtr;
typedef vtkSmartPointer<class vtkProp> vtkPropPtr;

namespace cx
{

typedef boost::shared_ptr<class OrientationAnnotation3DRep> OrientationAnnotation3DRepPtr;

/** \brief Class for display of an orientation annotation cube in 3D.
 * \ingroup cx_resource_view
 *
 */
class cxResourceVisualization_EXPORT OrientationAnnotation3DRep: public RepImpl
{
Q_OBJECT
public:
	static OrientationAnnotation3DRepPtr New(const QString& uid="");
	virtual ~OrientationAnnotation3DRep();
	virtual QString getType() const { return "OrientationAnnotation3DRep"; }

	bool getVisible() const;
	void setVisible(bool on);
	void setMarkerFilename(const QString filename); ///< empty and default means a cube
	void setSize(double size); ///< fraction of viewport to use

private slots:
protected:
	OrientationAnnotation3DRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

private:
	vtkOrientationMarkerWidgetPtr mMarker;
	double mSize;
	Vector3D mColor;
	std::pair<QString, vtkPropPtr> mMarkerCache; ///< cache all loaded markers in order to save load time. (not really necessary, leftover from when this was a static cache.)

	void rebuild(vtkRenderWindowInteractorPtr interactor);
	vtkAxesActorPtr createAxes();
	vtkAnnotatedCubeActorPtr createCube();
	vtkPropPtr readMarkerFromFile(const QString filename);
};

}

#endif /* CXORIENTATIONANNOTATION3DREP_H_ */

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

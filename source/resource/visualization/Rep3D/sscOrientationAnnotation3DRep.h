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

/*
 * sscOrientationAnnotation3DRep.h
 *
 *  \date Mar 24, 2011
 *      \author christiana
 */
#ifndef SSCORIENTATIONANNOTATION3DREP_H_
#define SSCORIENTATIONANNOTATION3DREP_H_

#include "sscRepImpl.h"
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"

typedef vtkSmartPointer<class vtkOrientationMarkerWidget> vtkOrientationMarkerWidgetPtr;
typedef vtkSmartPointer<class vtkAnnotatedCubeActor> vtkAnnotatedCubeActorPtr;
typedef vtkSmartPointer<class vtkProp> vtkPropPtr;

namespace cx
{

typedef boost::shared_ptr<class OrientationAnnotation3DRep> OrientationAnnotation3DRepPtr;

/** \brief Class for display of an orientation annotation cube in 3D.
 * \ingroup cx_resource_visualization
 *
 */
class OrientationAnnotation3DRep: public RepImpl
{
Q_OBJECT
public:
	static OrientationAnnotation3DRepPtr New(const QString& uid, const QString& name);
	virtual ~OrientationAnnotation3DRep();
	virtual QString getType() const { return "OrientationAnnotation3DRep"; }

	bool getVisible() const;
	void setVisible(bool on);
	void setMarkerFilename(const QString filename); ///< empty and default means a cube
	void setSize(double size); ///< fraction of viewport to use

private slots:
protected:
	OrientationAnnotation3DRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

private:
	vtkOrientationMarkerWidgetPtr mMarker;
	double mSize;
	Vector3D mColor;
	std::pair<QString, vtkPropPtr> mMarkerCache; ///< cache all loaded markers in order to save load time. (not really necessary, leftover from when this was a static cache.)

	void rebuild(vtkRenderWindowInteractorPtr interactor);
	vtkAxesActorPtr createAxes();
	vtkAnnotatedCubeActorPtr createCube();
	vtkPropPtr readMarkerFromFile(const QString filename);
	void reduceSTLFile(const QString source, const QString dest, double reduction);
};

}

#endif /* SSCORIENTATIONANNOTATION3DREP_H_ */

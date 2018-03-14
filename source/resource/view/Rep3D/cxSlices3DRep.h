/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSLICES3DREP_H_
#define CXSLICES3DREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include <vector>
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"
#include "cxDefinitions.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

//---------------------------------------------------------
namespace cx
{
typedef boost::shared_ptr<class Slices3DRep> Slices3DRepPtr;
typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/** \brief Display several slices through volumes in 3D.
 *
 * Use this as an alternative to full volume rendering.
 * Define a set of planes and an ordered set of images.
 * The images are sliced in a way similar to that in
 * Texture3DSlicerRep, but they are displayed in the
 * 3D View instead of in 2D.
 *
 * Used by CustusX.
 *
 * \sa Texture3DSlicerProxy
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT Slices3DRep: public RepImpl
{
Q_OBJECT
public:
	static Slices3DRepPtr New(SharedOpenGLContextPtr context, const QString& uid);
	virtual ~Slices3DRep();
	virtual QString getType() const { return "Slices3DRep"; }

	void setShaderPath(QString path);
	void setImages(std::vector<ImagePtr> images);
	void addPlane(PLANE_TYPE plane, PatientModelServicePtr dataManager);
	void setTool(ToolPtr tool);

protected:
	Slices3DRep(SharedOpenGLContextPtr context);
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

private:
	std::vector<Texture3DSlicerProxyPtr> mProxy;
	SharedOpenGLContextPtr mSharedOpenGLContext;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif /* CXSLICES3DREP_H_ */

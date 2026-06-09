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

#include <vector>
#include "cxRepImpl.h"
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"
#include "cxTransform3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class SliceProxyInterface> SliceProxyInterfacePtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class SlicedImageProxy> SlicedImageProxyPtr;
typedef boost::shared_ptr<class Slices3DRep> Slices3DRepPtr;

/**\brief Helper for rendering one slice of a 3D volume in 3D space.
 *
 * Combines a SliceProxy (plane transform), SlicedImageProxy (VTK reslice
 * pipeline) and a vtkImageSlice actor positioned via the slice-to-reference
 * transform (rMs).
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT Slice3DProxy : public QObject
{
	Q_OBJECT
public:
	static boost::shared_ptr<Slice3DProxy> New();
	~Slice3DProxy();

	void setSliceProxy(SliceProxyInterfacePtr sliceProxy);
	void setImage(ImagePtr image);
	vtkImageSlicePtr getActor();

private slots:
	void transformChangedSlot(Transform3D sMr);

private:
	Slice3DProxy();
	SliceProxyInterfacePtr mSliceProxy;
	SlicedImageProxyPtr mSlicer;
	vtkImageSlicePtr mActor;
};
typedef boost::shared_ptr<Slice3DProxy> Slice3DProxyPtr;

/**\brief Display 2D slices from a 3D volume positioned in the 3D view.
 *
 * Creates one image slice per plane type. Each slice is resliced from the
 * 3D volume data and positioned in 3D reference space using the SliceProxy
 * transform (rMs = sMr.inv()).
 *
 * \sa SliceProxy SlicedImageProxy
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT Slices3DRep : public RepImpl
{
	Q_OBJECT
public:
	static Slices3DRepPtr New(const QString& uid="");
	virtual ~Slices3DRep();
	virtual QString getType() const { return "Slices3DRep"; }

	void addPlane(PLANE_TYPE plane, PatientModelServicePtr dataManager);
	void setImages(std::vector<ImagePtr> images);
	void setTool(ToolPtr tool);

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

private:
	Slices3DRep();
	std::vector<Slice3DProxyPtr> mProxies;
	std::vector<SliceProxyPtr> mSliceProxies;
};

} // namespace cx

#endif /* CXSLICES3DREP_H_ */

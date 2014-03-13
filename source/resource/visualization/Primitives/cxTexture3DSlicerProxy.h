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

#ifndef CXTEXTURE3DSLICERPROXY_H_
#define CXTEXTURE3DSLICERPROXY_H_

#include "cxRepImpl.h"
#include <vector>
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

//---------------------------------------------------------
namespace cx
{

typedef vtkSmartPointer<class TextureSlicePainter> TextureSlicePainterPtr;
typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/**
 * \brief Helper class for GPU-based slicing.
 *
 * This class is a dummy that only provide interface.
 * For OS that don't implement the GPU slicer, use this dummy.
 * See Texture3DSlicerProxyImpl for the real implementation.
 *
 * \sa Texture3DSlicerProxyImpl for the real implementation.
 *
 * \ingroup cx_resource_visualization
 */
class Texture3DSlicerProxy: public QObject
{
Q_OBJECT
public:
	static Texture3DSlicerProxyPtr New();

	virtual ~Texture3DSlicerProxy() {}
	virtual void setShaderPath(QString shaderFile) {}
	virtual void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp) {}
	virtual void setImages(std::vector<ImagePtr> images) {}
	virtual std::vector<ImagePtr> getImages() { return std::vector<ImagePtr>(); }
	virtual void setSliceProxy(SliceProxyPtr slicer) {}
	virtual SliceProxyPtr getSliceProxy() { return SliceProxyPtr(); }
	virtual void update() {}
	virtual void setTargetSpaceToR(){}
	virtual vtkActorPtr getActor() { return vtkActorPtr(); }

	static bool isSupported(vtkRenderWindowPtr window);
};

#ifndef WIN32

/**
 * \brief Slice volumes using a SliceProxy.
 *
 * The functionality is equal to SlicedImageProxy, but the actual slicing
 * is performed by loading the image into the GPU as a 3D texture and
 * slicing it there, using the GPU.
 *
 * Used by Sonowand and Sintef.
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 *
 * \ingroup cx_resource_visualization
 */
class Texture3DSlicerProxyImpl: public Texture3DSlicerProxy
{
Q_OBJECT
public:
	static Texture3DSlicerProxyPtr New();
	virtual ~Texture3DSlicerProxyImpl();
	void setShaderPath(QString shaderFile);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ImagePtr> images);
	void setSliceProxy(SliceProxyPtr slicer);
	SliceProxyPtr getSliceProxy() { return mSliceProxy; }
	void update();
	void setTargetSpaceToR(); ///< use to draw the slice in 3D r space instead of in 2D s space.
	vtkActorPtr getActor();
	std::vector<ImagePtr> getImages() { return mImages; };

protected:
	Texture3DSlicerProxyImpl();
	void createGeometryPlane(Vector3D point1_s, Vector3D point2_s, Vector3D origin_s);

private slots:
	void transformChangedSlot();
	void updateColorAttributeSlot();
	void imageChanged();
//	void viewChanged();
private:
	void resetGeometryPlane();
	void updateCoordinates(int index);
	QString getTCoordName(int index);
	void setColorAttributes(int i);
	DoubleBoundingBox3D mBB_s;
	std::vector<ImagePtr> mImages;
	SliceProxyPtr mSliceProxy;
//	View* mView;
	bool mTargetSpaceIsR;

	TextureSlicePainterPtr mPainter;
	vtkActorPtr mActor;
	vtkPolyDataPtr mPolyData;
	vtkPlaneSourcePtr mPlaneSource;
	vtkPainterPolyDataMapperPtr mPainterPolyDatamapper;
};

#endif // WIN32


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* CXTEXTURE3DSLICERPROXY_H_ */

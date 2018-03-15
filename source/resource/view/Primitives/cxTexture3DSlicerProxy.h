/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXTEXTURE3DSLICERPROXY_H_
#define CXTEXTURE3DSLICERPROXY_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include <vector>
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxConfig.h"
#include "cxShaderCallback.h"

typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;
typedef vtkSmartPointer<class vtkOpenGLPolyDataMapper> vtkOpenGLPolyDataMapperPtr;

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
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT Texture3DSlicerProxy: public QObject
{
	Q_OBJECT
public:
	static Texture3DSlicerProxyPtr New(SharedOpenGLContextPtr context);

	virtual ~Texture3DSlicerProxy() {}
	virtual void setShaderPath(QString shaderFile) {}
	virtual void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp) {}
	virtual void setImages(std::vector<ImagePtr> images) {}
	virtual std::vector<ImagePtr> getImages()
	{
		return std::vector<ImagePtr>();
	}
	virtual void setSliceProxy(SliceProxyPtr slicer) {}
	virtual SliceProxyPtr getSliceProxy()
	{
		return SliceProxyPtr();
	}
	virtual void setTargetSpaceToR() {}
	virtual vtkActorPtr getActor()
	{
		return vtkActorPtr();
	}
	virtual void setRenderWindow(vtkRenderWindowPtr window) {};

	static bool isSupported(vtkRenderWindowPtr window)
	{
		return true;
	};

protected:
	Texture3DSlicerProxy() {};
};

//--------------------------------------------------------------------

/**
 * \brief Slice volumes using a SliceProxy.
 *
 * The functionality is equal to SlicedImageProxy, but the actual slicing
 * is performed by loading the image into the GPU as a 3D texture and
 * slicing it there, using the GPU.
 *
 *
 *
 * \ingroup cx_resource_view
 */

class cxResourceVisualization_EXPORT Texture3DSlicerProxyImpl: public Texture3DSlicerProxy
{
	Q_OBJECT

public:
	static Texture3DSlicerProxyPtr New(SharedOpenGLContextPtr context);

	virtual ~Texture3DSlicerProxyImpl();

	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ImagePtr> images);
	void setSliceProxy(SliceProxyPtr slicer);
	SliceProxyPtr getSliceProxy();
	void setTargetSpaceToR(); ///< use to draw the slice in 3D r space instead of in 2D s space.
	vtkActorPtr getActor();
	std::vector<ImagePtr> getImages();
	virtual void setRenderWindow(vtkRenderWindowPtr window);

protected slots:
	void transformChangedSlot();
	void transferFunctionChangedSlot();
	void uploadChangedImage(QString uid);

protected:
	Texture3DSlicerProxyImpl(SharedOpenGLContextPtr context);
	void createGeometryPlane(Vector3D point1_s, Vector3D point2_s, Vector3D origin_s);

private:
	void resetGeometryPlane();

	void updateAndUploadImages(std::vector<ImagePtr> new_images_raw);
	void updateAndUploadCoordinates();
	void updateAndUploadColorAttribute();

	void uploadImagesToSharedContext(std::vector<ImagePtr> images, SharedOpenGLContextPtr sharedOpenGLContext, ShaderCallbackPtr shaderCallback) const;
	void uploadTextureCoordinatesToSharedContext(QString image_uid, vtkFloatArrayPtr textureCoordinates, SharedOpenGLContextPtr sharedOpenGLContext, ShaderCallbackPtr shaderCallback) const;
	void uploadColorAttributesToSharedContext(QString imageUid, float llr, vtkLookupTablePtr lut, float window, float level, float alpha, SharedOpenGLContextPtr sharedOpenGLContext, ShaderCallbackPtr shaderCallback) const;

	QString getTCoordName(int index);
	void setColorAttributes(int i);
	std::vector<ImagePtr> convertToUnsigned(std::vector<ImagePtr> images_raw);

	bool isNewInputImages(std::vector<ImagePtr> images_raw);

	QString generateTextureCoordinateName(QString imageUid) const;
	void generateAndSetShaders();


	SharedOpenGLContextPtr mSharedOpenGLContext;
	ShaderCallbackPtr mShaderCallback;
	QString mUid;

	DoubleBoundingBox3D mBB_s;
	std::vector<ImagePtr> mImages;
	std::vector<ImagePtr> mRawImages;
	SliceProxyPtr mSliceProxy;
	bool mTargetSpaceIsR;

	vtkActorPtr mActor;
	vtkPolyDataPtr mPolyData;
	vtkPlaneSourcePtr mPlaneSource;
	vtkPolyDataAlgorithmPtr mPolyDataAlgorithm;
	vtkOpenGLPolyDataMapperPtr mOpenGLPolyDataMapper;
	vtkRenderWindowPtr mCurrentRenderWindow;

	vtkFloatArrayPtr mTextureCoordinates;

};
//--------------------------------------------------------------------

}//namespace cx

#endif /* CXTEXTURE3DSLICERPROXY_H_ */

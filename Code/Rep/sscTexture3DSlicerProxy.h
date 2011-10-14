#ifndef SSCTEXTURE3DSLICERPROXY_H_
#define SSCTEXTURE3DSLICERPROXY_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

//---------------------------------------------------------
namespace ssc
{

typedef vtkSmartPointer<class TextureSlicePainter> TextureSlicePainterPtr;
typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/**
 * \class Texture3DSlicerProxy
 *
 * \brief Slice a volume using a SliceProxy.
 *
 * The functionality is equal to SlicedImageProxy, but the actual slicing
 * is performed by loading the image into the GPU as a 3D texture and
 * slicing it there, using the GPU.
 *
 * Used by Sonowand and Sintef
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 */
class Texture3DSlicerProxy: public QObject
{
Q_OBJECT
public:
	static Texture3DSlicerProxyPtr New();
	virtual ~Texture3DSlicerProxy();
	void setShaderFile(QString shaderFile);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ssc::ImagePtr> images);
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	ssc::SliceProxyPtr getSliceProxy() { return mSliceProxy; }
	void update();
	void setTargetSpaceToR(); ///< use to draw the slice in 3D r space instead of in 2D s space.
	vtkActorPtr getActor();

protected:
	Texture3DSlicerProxy();
	void createGeometryPlane(Vector3D point1_s, Vector3D point2_s, Vector3D origin_s);

private slots:
	void transformChangedSlot();
	void updateColorAttributeSlot();
//	void viewChanged();
private:
	void resetGeometryPlane();
	void updateCoordinates(int index);
	QString getTCoordName(int index);
	void setColorAttributes(int i);
	DoubleBoundingBox3D mBB_s;
	std::vector<ssc::ImagePtr> mImages;
	ssc::SliceProxyPtr mSliceProxy;
//	View* mView;
	bool mTargetSpaceIsR;

	TextureSlicePainterPtr mPainter;
	vtkActorPtr mActor;
	vtkPolyDataPtr mPolyData;
	vtkPlaneSourcePtr mPlaneSource;
	vtkPainterPolyDataMapperPtr mPainterPolyDatamapper;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURE3DSLICERPROXY_H_ */

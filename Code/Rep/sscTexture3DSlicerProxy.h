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

/**abstract class design to solve windows compile problems.
 * See Texture3DSlicerProxyImpl for the real implementation
 *
 */
class Texture3DSlicerProxy: public QObject
{
Q_OBJECT
public:
	static Texture3DSlicerProxyPtr New();

	virtual ~Texture3DSlicerProxy() {}
	virtual void setShaderFile(QString shaderFile) {}
	virtual void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp) {}
	virtual void setImages(std::vector<ssc::ImagePtr> images) {}
	virtual void setSliceProxy(ssc::SliceProxyPtr slicer) {}
	virtual ssc::SliceProxyPtr getSliceProxy() { return ssc::SliceProxyPtr(); }
	virtual void update() {}
	virtual void setTargetSpaceToR(){}
	virtual vtkActorPtr getActor() { return vtkActorPtr(); }
};

#ifndef WIN32

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
class Texture3DSlicerProxyImpl: public Texture3DSlicerProxy
{
Q_OBJECT
public:
	static Texture3DSlicerProxyPtr New();
	virtual ~Texture3DSlicerProxyImpl();
	void setShaderFile(QString shaderFile);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ssc::ImagePtr> images);
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	ssc::SliceProxyPtr getSliceProxy() { return mSliceProxy; }
	void update();
	void setTargetSpaceToR(); ///< use to draw the slice in 3D r space instead of in 2D s space.
	vtkActorPtr getActor();

protected:
	Texture3DSlicerProxyImpl();
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

#endif // WIN32


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURE3DSLICERPROXY_H_ */

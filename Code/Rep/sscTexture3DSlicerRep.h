/*
 * vmTexture3DSlicerRep.h
 *
 *  Created on: Oct 13, 2009
 *      Author: petterw
 */

#ifndef SSCTEXTURE3DSLICER_H_
#define SSCTEXTURE3DSLICER_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkPainterPolyDataMapper> vtkPainterPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkFloatArray> vtkFloatArrayPtr;
typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkStripper> vtkStripperPtr;
typedef vtkSmartPointer<class vtkPlaneSource> vtkPlaneSourcePtr;

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------
// forward declarations
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef vtkSmartPointer<class TextureSlicePainter > TextureSlicePainterPtr;

typedef boost::shared_ptr<class Texture3DSlicerRep> Texture3DSlicerRepPtr;

/**Slice a volume using a SliceProxy.
 *
 * The functionality is equal to SlicerRepSW, but the actual slicing
 * is performed by loading the image into the GPU as a 3D texture and
 * slicing it there, using the GPU.
 *
 * Used by Sonowand.
 */
class Texture3DSlicerRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static Texture3DSlicerRepPtr New(const std::string& uid);
	virtual ~Texture3DSlicerRep();
	virtual std::string getType() const
	{
		return "vm::Texture3DSlicerRep";
	}
	virtual void printSelf(std::ostream & os, ssc::Indent indent);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp);
	void setImages(std::vector<ssc::ImagePtr> images);
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	void update();
protected:
	Texture3DSlicerRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	void createGeometryPlane(Vector3D point1_s, Vector3D point2_s, Vector3D origin_s);

private slots:
	void sliceTransformChangedSlot(Transform3D sMr);
	void updateColorAttributeSlot();
private:
	void createCoordinates(int count);
	void updateCoordinates(int index);
	std::string getTCoordName(int index);
	void setColorAttributes(int i);
	DoubleBoundingBox3D mBB_s;
	std::vector<ssc::ImagePtr> mImages;
	ssc::SliceProxyPtr mSliceProxy;

	TextureSlicePainterPtr mPainter;
	vtkActorPtr mActor;
	vtkPolyDataPtr mPolyData;
	vtkPlaneSourcePtr mPlaneSource;
	vtkStripperPtr mStripper;
	vtkPainterPolyDataMapperPtr mPainterPolyDatamapper;

	//std::vector<testApp::SlicedImageProxyPtr> mSlices;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURE3DSLICER_H_ */


#ifndef SSCTEXTURE3DSLICER_H_
#define SSCTEXTURE3DSLICER_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

//---------------------------------------------------------
namespace ssc
{

typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/**
 * \class Texture3DSlicerRep
 *
 * \brief Slice a volume using a SliceProxy.
 *
 * The functionality is equal to SlicerRepSW, but the actual slicing
 * is performed by loading the image into the GPU as a 3D texture and
 * slicing it there, using the GPU.
 *
 * Used by Sonowand.
 *
 * \date Oct 13, 2009
 * \date petterw
 */
class Texture3DSlicerRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static Texture3DSlicerRepPtr New(const QString& uid);
	virtual ~Texture3DSlicerRep();
	virtual QString getType() const
	{
		return "ssc::Texture3DSlicerRep";
	}
	void setShaderFile(QString shaderFile);
	virtual void printSelf(std::ostream & os, ssc::Indent indent);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ssc::ImagePtr> images);
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	void update();
	void setTargetSpaceToR();

protected:
	Texture3DSlicerRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

private slots:
	void viewChanged();
private:
	View* mView;
	Texture3DSlicerProxyPtr mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURE3DSLICER_H_ */


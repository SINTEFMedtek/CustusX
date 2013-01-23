// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2012- SINTEF Medical Technology
// Copyright (C) 2012- Sonowand AS
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

#ifndef SSCGPURAYCASTVOLUMEREP_H_
#define SSCGPURAYCASTVOLUMEREP_H_

#ifndef WIN32

#include "sscRepImpl.h"
#include <vector>
#include <QSet>
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

#define MAX_CONCURRENT_VOLUMES 4
//---------------------------------------------------------
typedef vtkSmartPointer<class vtkCubeSource> vtkCubeSourcePtr;
class TestGPURayCaster;

namespace ssc
{
typedef vtkSmartPointer<class GPURayCastVolumePainter> GPURayCastVolumePainterPtr;

/**
 * \brief Multi-Volume GPU raycast renderer
 *
 * This class uses a ray casting algorthim implemented on the GPU to
 * render a representation of one or more volumes in the 3D scene.
 * 
 * \ingroup sscRep3D
 */
class GPURayCastVolumeRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static GPURayCastVolumeRepPtr New(const QString& uid);
	virtual ~GPURayCastVolumeRep();
	virtual QString getType() const
	{
		return "ssc::GPURayCastVolumeRep";
	}
	/**
	 * \brief Override the default locations of the shader files
	 */
	void setShaderFolder(QString folder);
	virtual void printSelf(std::ostream & os, ssc::Indent indent);
	/**
	 * \brief Set the images to render using this Rep
	 */
	void setImages(std::vector<ssc::ImagePtr> images);
	/**
	 * \brief Get the images rendered with this class
	 * At most ssc::GPURayCastVolumePainter::maxVolumes images will be used, any additional images in the list will be ignored
	 */
	std::vector<ssc::ImagePtr> getImages();
	/**
	 * \brief Set a clip plane for this renderer. Only the plane definition of the clipper is used, it is not used to determine which volumes to clip.
	 * \sa setClipVolumes
	 */
	void setClipper(SlicePlaneClipperPtr clipper);
	/**
	 * \brief Set which volumes to clip using the clipper set with \r setClipper
	 */
	void setClipVolumes(QStringList volumes);
	/**
	 * \brief Override default step size.
	 * Set the sampling distance along each ray in mm. Lower values gives better quality but slower performance. Default value is 1.0
	 */
	void setStepSize(double stepsize);
	/**
	 * \brief Set the rendering mode
	 */
	void enableImagePlaneDownsampling(int maxPixels);
	void disableImagePlaneDownsampling();
	/**
	 * \brief Check that the rep is supported on this plaform/renderer
	 */
	static bool isSupported(vtkRenderWindowPtr window);

protected:
	GPURayCastVolumeRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

private slots:
	void updateColorAttributeSlot();
	void transformChangedSlot();
private:
	DoubleBoundingBox3D mBB_s;
	std::vector<ssc::ImagePtr> mImages;
	QSet<QString> mClipVolumes;

	GPURayCastVolumePainterPtr mPainter;
	vtkActorPtr mActor;
	boost::array<vtkTransformPolyDataFilterPtr, MAX_CONCURRENT_VOLUMES> mTransformPolyData;
	vtkAppendPolyDataPtr mMerger;
	vtkPainterPolyDataMapperPtr mPainterPolyDatamapper;
	SlicePlaneClipperPtr mClipper;
	static const double defaultStepSize;
	friend class ::TestGPURayCaster;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif // WIN32

#endif /* SSCGPURAYCASTVOLUMEREP_H_ */


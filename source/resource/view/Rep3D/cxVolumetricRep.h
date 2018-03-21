/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXVOLUMETRICREP_H_
#define CXVOLUMETRICREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"


namespace cx
{
	typedef boost::shared_ptr<class VolumeProperty> VolumePropertyPtr;
	typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;
}

namespace cx
{


/** \brief Display a volume in 3D
 * 
 * This abstract interface is implemented by VolumetricRep
 * Previously: ProgressiveLODVolumetricRep, which is removed.
 * 
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT VolumetricBaseRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~VolumetricBaseRep() {}

	virtual void setImage(ImagePtr image) = 0; ///< set the reps image
	virtual ImagePtr getImage() = 0; ///< get the reps image
	virtual bool hasImage(ImagePtr image) const = 0; ///< check if the reps has the image
	virtual vtkVolumePtr getVtkVolume() = 0; ///< get the images vtkVolume
	//virtual void setResampleFactor(double factor) = 0; ///< set how detailed the image should be
	virtual void setMaxVolumeSize(long maxVoxels) = 0; ///< set max volume size for rendering. Must be set before setImage()
protected:
	VolumetricBaseRep() : RepImpl() {}
};

/** \brief Display a volume in 3D
 *
 * Use this to render volumetric image data in a 3D scene. Both
 * texture rendering and GPU raycasting are available.
 * 
 * Used by Sonowand.
 * Used by CustusX.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT VolumetricRep : public VolumetricBaseRep
{
	Q_OBJECT
public:
	virtual ~VolumetricRep();

	static VolumetricRepPtr New(QString uid="") { return wrap_new(new VolumetricRep(), uid); }
	virtual QString getType() const { return "VolumetricRep"; }
	virtual void setImage(ImagePtr image); ///< set the reps image
	virtual ImagePtr getImage(); ///< get the reps image
	virtual bool hasImage(ImagePtr image) const; ///< check if the reps has the image
	virtual vtkVolumePtr getVtkVolume() { return mVolume; } ///< get the images vtkVolume
	void setMaxVolumeSize(long maxVoxels); ///< set max volume size for rendering. Must be set before setImage()
	void setUseGPUVolumeRayCastMapper();
	void setUseVolumeTextureMapper();

protected:
	VolumetricRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

	cx::VolumePropertyPtr mVolumeProperty;
	vtkVolumeMapperPtr mMapper;
	vtkVolumePtr mVolume;
	long mMaxVoxels; ///< always resample volume below this size.

	ImagePtr mImage;
	cx::ImageMapperMonitorPtr mMonitor; ///< helper object for visualizing clipping/cropping

protected slots:
	void transformChangedSlot();
	void vtkImageDataChangedSlot();
	void updateVtkImageDataSlot();
};
//---------------------------------------------------------
} // namespace cx
//---------------------------------------------------------

#endif /*CXVOLUMETRICREP_H_*/

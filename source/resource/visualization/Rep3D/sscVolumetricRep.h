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

#ifndef SSCVOLUMEREP_H_
#define SSCVOLUMEREP_H_

#include "sscRepImpl.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"


namespace cx
{
	typedef boost::shared_ptr<class VolumeProperty> VolumePropertyPtr;
	typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;
}

namespace cx
{


/**\brief Display a volume in 3D
 * 
 * This abstract interface is implemented by VolumetricRep
 * Previously: ProgressiveLODVolumetricRep, which is removed.
 * 
 * \ingroup sscRep
 * \ingroup sscRep3D
 */
class VolumetricBaseRep : public RepImpl
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
signals:
	void internalVolumeChanged(); ///< emitted when the internal vtkVolume has been replaced with a new one (happens with ProgressiveLODVolumetricRep)
protected:
	VolumetricBaseRep() : RepImpl() {}
};

/**\brief Display a volume in 3D
 *
 * Use this to render volumetric image data in a 3D scene. Both
 * texture rendering and GPU raycasting are available.
 * 
 * Used by Sonowand.
 * Used by CustusX.
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
 */
class VolumetricRep : public VolumetricBaseRep
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
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

	cx::VolumePropertyPtr mVolumeProperty;
	vtkVolumeMapperPtr mMapper;
	vtkVolumePtr mVolume;
	long mMaxVoxels; ///< always resample volume below this size.

	ImagePtr mImage;
	cx::ImageMapperMonitorPtr mMonitor; ///< helper object for visualizing clipping/cropping

private slots:
	void transformChangedSlot();
	void vtkImageDataChangedSlot();
};
//---------------------------------------------------------
} // namespace cx
//---------------------------------------------------------

#endif /*SSCVOLUMEREP_H_*/

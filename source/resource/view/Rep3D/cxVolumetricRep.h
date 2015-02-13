/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
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
signals:
	void internalVolumeChanged(); ///< emitted when the internal vtkVolume has been replaced with a new one (happens with ProgressiveLODVolumetricRep)
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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
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
};
//---------------------------------------------------------
} // namespace cx
//---------------------------------------------------------

#endif /*CXVOLUMETRICREP_H_*/

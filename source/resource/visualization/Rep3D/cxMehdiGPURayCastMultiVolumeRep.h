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
#ifndef CXMEHDIGPURAYCASTMULTIVOLUMEREP_H
#define CXMEHDIGPURAYCASTMULTIVOLUMEREP_H

#include "cxRepImpl.h"
#include "cxForwardDeclarations.h"
#include <vector>
#include "vtkForwardDeclarations.h"

#include "cxConfig.h"

#include "cxImageMapperMonitor.h"

typedef vtkSmartPointer<class vtkOpenGLGPUMultiVolumeRayCastMapper> vtkOpenGLGPUMultiVolumeRayCastMapperPtr;


namespace cx
{


typedef boost::shared_ptr<class ImageEnveloper> ImageEnveloperPtr;
typedef boost::shared_ptr<class MehdiGPURayCastMultiVolumeRep> MehdiGPURayCastMultiVolumeRepPtr;
typedef boost::shared_ptr<class VolumeProperty> VolumePropertyPtr;
typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;

class MehdiGPURayCastMultiVolumeRepImageMapperMonitor : public ImageMapperMonitor
{
public:
	static ImageMapperMonitorPtr create(vtkVolumePtr volume, ImagePtr image, int volumeIndex);
protected:
	virtual void clearClipping();
	/** Warning: The mapper supports only one plane. Only the first plane from Image is used for clip.
	  */
	virtual void applyClipping();
	virtual void applyCropping();
	MehdiGPURayCastMultiVolumeRepImageMapperMonitor(vtkVolumePtr volume, ImagePtr image, int volumeIndex);
private:
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr getMehdiMapper();
	int mVolumeIndex;
};


/** Hack: ensures that the QObject part of MehdiGPURayCastMultiVolumeRep is moced
  * irrespective of the value of CX_BUILD_MEHDI_VTKMULTIVOLUME.
  * The CX_BUILD_MEHDI_VTKMULTIVOLUME define is not handled correctly by moc.
  *
  */
class MehdiGPURayCastMultiVolumeRepBase: public RepImpl
{
Q_OBJECT
public:
	MehdiGPURayCastMultiVolumeRepBase();
	virtual ~MehdiGPURayCastMultiVolumeRepBase() {}
	virtual void setMaxVolumeSize(long maxVoxels);

protected:
	long mMaxVoxels; ///< always resample volume below this size.

private slots:
	virtual void transformChangedSlot() = 0;
	virtual void vtkImageDataChangedSlot() = 0;
};

#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

/** 
 * Rep wrapping the multivolume renderer created by Mehdi.
 *
 * \ingroup cx_resource_visualization
 * \date 9 Sep 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class MehdiGPURayCastMultiVolumeRep: public MehdiGPURayCastMultiVolumeRepBase
{
	Q_OBJECT
public:
	static MehdiGPURayCastMultiVolumeRepPtr New(QString uid="") { return wrap_new(new MehdiGPURayCastMultiVolumeRep(), uid); }
	virtual ~MehdiGPURayCastMultiVolumeRep();

	virtual QString getType() const { return "MehdiGPURayCastMultiVolumeRep"; }

	void setImages(std::vector<ImagePtr> images);

protected:
	MehdiGPURayCastMultiVolumeRep();
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

private slots:
	void transformChangedSlot();
	void vtkImageDataChangedSlot();

private:
	void initializeMapper();
	void updateTransforms();
	void clearVolume();
	void setupVolume();
	ImagePtr getEnvelopingImage();
	void disconnectImages();
	void connectImages();
	void setupMonitor();
	void setupVolumeProperties();
	void setupReferenceVolumeAndPropertiesAndConnectToVolume();

	vtkVolumePtr mVolume;
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mMapper;
	std::vector<VolumePropertyPtr> mVolumeProperties;
	VolumePropertyPtr mReferenceProperty;
	std::vector<ImagePtr> mImages;
	ImagePtr mReferenceImage;
	std::vector<ImageMapperMonitorPtr> mMonitors;
};

#endif // CXMEHDIGPURAYCASTMULTIVOLUMEREP_H


} // namespace cx


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME



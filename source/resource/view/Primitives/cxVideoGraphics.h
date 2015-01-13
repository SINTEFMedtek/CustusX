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


#ifndef CXVIDEOGRAPHICS_H_
#define CXVIDEOGRAPHICS_H_

#include "cxResourceVisualizationExport.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

typedef vtkSmartPointer<class vtkTransformTextureCoords> vtkTransformTextureCoordsPtr;
typedef vtkSmartPointer<class vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<class vtkImageMask> vtkImageMaskPtr;
typedef vtkSmartPointer<class UltrasoundSectorSource> UltrasoundSectorSourcePtr;

namespace cx
{
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;

/** \brief Wrap vtkActor displaying a video image, possibly clipped by a sector.
 *
 * Create a pipeline from a video vtkImageData to a vtkActor.
 * Clip using the mask or clip methods.
 * Can be used directly in 2D, or in 3D by setting the actor user matrix.
 *
 * \ingroup cx_resource_visualization
 *
 * \date april 26, 2013
 * \author christiana
 */
class cxResourceVisualization_EXPORT VideoGraphics
{
public:
	VideoGraphics();
	virtual ~VideoGraphics();

	vtkActorPtr getActor();
	void setActorUserMatrix(vtkMatrix4x4Ptr rMu);
	void setVisibility(bool visible);
	/** Set flag to flip the output in the vertical direction,
	  * default = true
	  * Legacy function supporting old functionality.
	  */
	void setFlipVertical(bool on);
	bool getFlipVertical();

	/** Set a mask for the video image.
	  * Only one of clear and clip can be active at a time.
	  * The mask must be of the same size as the video.
	  * Zeros in the mask will be set to transparent.
	  */
	void setMask(vtkImageDataPtr mask);

	/** Set a clip polygon for the video image.
	  * Only one of clear and clip can be active at a time.
	  * The mask must be of the same size as the video.
	  * Zeros in the mask will be set to transparent.
	  */
	void setClip(vtkPolyDataPtr sector);

	/** Set the imagedata pointing to the video.
	  */
	void setInputVideo(vtkImageDataPtr video);

	/** One of the previously set inputs have been modified. Update the pipeline.
	  */
	void update();

private:
	void setLookupTable();

	void setupPipeline();
	void connectVideoImageToPipeline();
	void updatePlaneSourceBounds();
	void updateLUT();
	bool inputImageIsEmpty();

	vtkImageDataPtr mInputMask;
	vtkPolyDataPtr mInputSector;
	vtkImageDataPtr mInputVideo;

	vtkLookupTablePtr mLUT;
	vtkImageChangeInformationPtr mDataRedirecter;
	vtkActorPtr mPlaneActor;
	vtkPlaneSourcePtr mPlaneSource;
	vtkTexturePtr mTexture;
	UltrasoundSectorSourcePtr mUSSource;
	vtkDataSetMapperPtr mDataSetMapper;
	vtkTransformTextureCoordsPtr mTransformTextureCoords;
	vtkTextureMapToPlanePtr mTextureMapToPlane;

	vtkImageThresholdPtr mMapZeroToOne;
	vtkImageMaskPtr mMaskFilter;
};
typedef boost::shared_ptr<VideoGraphics> VideoGraphicsPtr;

} // namespace cx

#endif // CXVIDEOGRAPHICS_H_

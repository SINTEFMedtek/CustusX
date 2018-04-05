/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_view
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

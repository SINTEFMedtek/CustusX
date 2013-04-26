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

#ifndef SSCVIDEOGRAPHICS_H
#define SSCVIDEOGRAPHICS_H

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

typedef vtkSmartPointer<class vtkTransformTextureCoords> vtkTransformTextureCoordsPtr;
typedef vtkSmartPointer<class vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<class vtkImageMask> vtkImageMaskPtr;
typedef vtkSmartPointer<class UltrasoundSectorSource> UltrasoundSectorSourcePtr;

namespace ssc
{
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;

/** \brief Wrap vtkActor displaying a video image, possibly clipped by a sector.
 *
 * Create a pipeline from a video vtkImageData to a vtkActor.
 * Clip using the mask or clip methods.
 * Can be used directly in 2D, or in 3D by setting the actor user matrix.
 *
 * \ingroup sscProxy
 *
 * \date april 26, 2013
 * \author christiana
 */
class VideoGraphics
{
public:
	VideoGraphics();
	virtual ~VideoGraphics();

	vtkActorPtr getActor();
	void setActorUserMatrix(vtkMatrix4x4Ptr rMu);
	void setVisibility(bool visible);

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

} // namespace ssc

#endif // SSCVIDEOGRAPHICS_H

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

#ifndef SSCLAYERSLICEREP_H_
#define SSCLAYERSLICEREP_H_

#include <vector>
#include "sscRepImpl.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class SlicedImageProxy> SlicedImageProxyPtr;
typedef boost::shared_ptr<class Image> ImagePtr;

typedef boost::shared_ptr<class BlendedSliceRep> BlendedSliceRepPtr;

/**\brief Slice a list of overlay volumes.
 *        NOT IN USE.
 *
 * The volumes are blended together during the slice process. The
 * SliceProxy handles the actual slice definition.
 * 
 * Used as the slicer for multi-layer slicing in Sonowand 2.1.
 * Replaced by Texture3DSlicerRep.
 *
 * \ingroup sscRep
 * \ingroup sscNotUsed
 * \ingroup sscRep2D
 */
class BlendedSliceRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~BlendedSliceRep();
	static BlendedSliceRepPtr New(const QString& uid);
	virtual QString getType() const { return "ssc::LayerSliceRep"; }
	void setSliceProxy(SliceProxyPtr slicer); 
	void setImages(std::vector<ImagePtr> images);
	void update();
protected:
	BlendedSliceRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);
	void addInputImages(vtkImageDataPtr slicedImage);

private slots:
	void updateAlphaSlot();

private:
	double getAlpha(int countImage);
	bool firstImage;
	vtkImageDataPtr mBaseImages;
	SliceProxyPtr mSlicer;
	ssc::SlicedImageProxyPtr mSlicedProxy;
	std::vector<ssc::SlicedImageProxyPtr> mSlices;

	vtkImageBlendPtr mBlender;
	int countImage;
	vtkImageActorPtr mImageActor;
};

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------









#endif /*SSCLAYERSLICEREP_H_*/

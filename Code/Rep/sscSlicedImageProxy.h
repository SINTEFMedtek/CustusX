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

#ifndef SSCSLICEDIMAGEPROXY_H_
#define SSCSLICEDIMAGEPROXY_H_

#include "sscRepImpl.h"
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;

typedef boost::shared_ptr<class SlicedImageProxy> SlicedImageProxyPtr;
typedef boost::shared_ptr<class ApplyLUTToImage2DProxy> ApplyLUTToImage2DProxyPtr;

/** \brief Helper class for applying sscLUT2D to an image.
 *
 * Input a 2D image and a lut.
 * Run the image through the lut using a vtkImageMapToColors
 * and return the output.
 *
 * For color input images, the lut is still applied, but is applied
 * to each color component (i.e. relative color distribution is not changed)
 *
 * Used internally in SlicedImageProxy .
 *
 * Used by CustusX
 *
 * \ingroup sscProxy
 */
class ApplyLUTToImage2DProxy : public QObject
{
Q_OBJECT
public:
	ApplyLUTToImage2DProxy();
	virtual ~ApplyLUTToImage2DProxy();
	void setImage(vtkImageDataPtr image, vtkLookupTablePtr lut);
	void setLut(vtkLookupTablePtr lut);
	vtkImageDataPtr getOutput();

private:
	vtkImageDataPtr createDummyImageData();

	vtkImageDataPtr mDummyImage; ///< need this to fool the vtk pipeline when no image is set
	vtkImageAlgorithmPtr mRedirecter;
	vtkImageMapToColorsPtr mWindowLevel;
};

/**\brief Helper class for slicing an image given a SliceProxy and an image.
 *
 * The image is sliced in software using the slice definition from
 * the SliceProxy
 *
 * Used internally by BlendedSliceRep and SlicerRepSW as the slice engine.
 * 
 * Used by Sonowand 2.1
 * Used by CustusX
 *
 * \ingroup sscProxy
 */
class SlicedImageProxy : public QObject
{
Q_OBJECT
public:
	SlicedImageProxy();
	virtual ~SlicedImageProxy();
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	void setImage(ImagePtr image);
	ImagePtr getImage() const;
	void update();
	vtkImageDataPtr getOutput(); ///< output 2D sliced image
	void printSelf(std::ostream & os, Indent indent);

private slots:
	void transformChangedSlot();
	void transferFunctionsChangedSlot();

private: 
	ApplyLUTToImage2DProxyPtr mImageWithLUTProxy;

//	vtkImageDataPtr createDummyImageData();

	SliceProxyPtr mSlicer;
//	vtkImageDataPtr mDummyImage; ///< need this to fool the vtk pipeline when no image is set
	ImagePtr mImage;
	//vtkImageMapToWindowLevelColorsPtr mWindowLevel;
//	vtkImageAlgorithmPtr mRedirecter;
 	//vtkImageReslicePtr mRedirecter;

//	vtkImageMapToColorsPtr mWindowLevel;
	vtkImageReslicePtr mReslicer;
	vtkMatrix4x4Ptr mMatrixAxes;
};

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /*SSCSLICEDIMAGEPROXY_H_*/

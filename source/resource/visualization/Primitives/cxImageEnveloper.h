// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXMULTIVOLUMEBOUNDINGBOX_H
#define CXMULTIVOLUMEBOUNDINGBOX_H

#include "cxForwardDeclarations.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include <vector>

namespace cx
{

typedef boost::shared_ptr<class ImageEnveloper> ImageEnveloperPtr;
/**
 * Generate a total bounding volume from a set of volumes.
 *
 * \ingroup cxServiceVisualizationRep
 * \date 3 Oct 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class ImageEnveloper
//class ImageEnvelopeGenerator
{
public:
	ImageEnveloper() {}
	virtual ~ImageEnveloper() {}

	virtual void setImages(std::vector<ImagePtr> images) = 0;
	//virtual Box getBox() const = 0;
	virtual ImagePtr getEnvelopingImage() = 0;
};

typedef boost::shared_ptr<class ImageEnveloperMoc> ImageEnveloperMocPtr;
class ImageEnveloperMoc : public ImageEnveloper
{
public:
	static ImageEnveloperMocPtr create();
	ImageEnveloperMoc() {}
	virtual ~ImageEnveloperMoc() {}
	virtual void setImages(std::vector<ImagePtr> images);
	//virtual Box getBox() const;
	virtual ImagePtr getEnvelopingImage();

private:
	std::vector<ImagePtr> mImages;
};

typedef boost::shared_ptr<class ImageEnveloperImpl> ImageEnveloperImplPtr;
class ImageEnveloperImpl : public ImageEnveloper
{
public:
	static ImageEnveloperImplPtr create();
	ImageEnveloperImpl() {}
	virtual ~ImageEnveloperImpl() {}

	virtual void setImages(std::vector<ImagePtr> images);
	//virtual Box getBox() const;
	virtual ImagePtr getEnvelopingImage();

private:
	std::vector<ImagePtr> mImages;

	struct Parameters
	{
		Transform3D m_rMd;
		Eigen::Array3i mDim;
		Vector3D mSpacing;
		QString mParentVolume;
	};

	ImageEnveloperImpl::Parameters createEnvelopeParametersFromImage(ImagePtr img);
	ImagePtr createEnvelopeFromParameters(Parameters box);
//	ImagePtr createEnvelopeFromImage(ImagePtr img);
};

} // namespace cx

#endif // CXMULTIVOLUMEBOUNDINGBOX_H

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

#ifndef SSCIMAGEBLENDERPROXY_H_
#define SSCIMAGEBLENDERPROXY_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <vector>

#include "vtkForwardDeclarations.h"

//----------------------------------------------------------
namespace ssc
{
//----------------------------------------------------------
typedef boost::shared_ptr<class Image> ImagePtr;

typedef boost::shared_ptr<class ImageBlenderProxy> ImageBlenderProxyPtr;

/**\brief Test implementation
 *        NOT IN USE.
 *
 * Part of sonowand test implementation of image blending.
 *
 * \ingroup sscProxy
 * \ingroup sscNotUsed
 */
class ImageBlenderProxy : public QObject
{
	Q_OBJECT

public:
	ImageBlenderProxy();
	virtual ~ImageBlenderProxy();
	virtual QString getType() const { return "ssc::ImageBlenderProxy"; }
	bool hasImage(ImagePtr image) const;
	void clearImages();
	void addImage(ImagePtr image);
	vtkImageDataPtr getOutput();
	std::vector<ImagePtr> getImages( ) { return mImages; };
	void removeImage(ImagePtr image);
	void createOverlay();
	void updateBlender();
	void updateAlpha();
	void sliceFirstTheneBlend();
	void volumeBlend();
	void addSlice(vtkImageDataPtr slice);
private:
	bool firstImage;
	vtkImageDataPtr baseImage;
	vtkImageBlendPtr mBlender;
	std::vector<ImagePtr> mImages;
	QString mImageUid;

	vtkWindowLevelLookupTablePtr mLookupTable;
	vtkImageMapToWindowLevelColorsPtr mWindowLevel;
	vtkImageMapToWindowLevelColorsPtr mColorMapper;
	vtkImageThreshold* mTreshold;
};

//----------------------------	------------------------------
}//end namespace
//----------------------------------------------------------
#endif /*SSCIMAGEBLENDERPROXY_H_*/

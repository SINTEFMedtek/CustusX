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
#ifndef CXIMAGEDATACONTAINER_H
#define CXIMAGEDATACONTAINER_H

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include <vector>

namespace cx
{

/**
 * \addtogroup cxResourceUtilities
 * \{
 */

/** Delayed loading of one vtkImageData.
 *
 * The data might not exist on disk at the time of construction, in this case use the
 * setExistsOnDisk() when it becomes available.
 *
 * \ingroup cxResourceUtilities
 * \date Dec 04 2012
 * \author christiana
 */
class CachedImageData
{
public:
	/**
	  * Initialize with a given image file
	  */
	explicit CachedImageData(QString filename, vtkImageDataPtr image = NULL, bool existsOnDisk = true);
	/**
	  * Return name of file.
	  */
	QString getFilename() { return mFilename; }
	vtkImageDataPtr getImage();
	/**
	  * Call to inform class that data has been saved to disk meaning
	  * that the data can be cleared from memory if necessary.
	  */
	void setExistsOnDisk(bool on);
	/**
	  * Clear the image contents from memory, if possible.
	  * Return true if purge was successful.
	  */
	bool purge();
private:
	bool mExistsOnDisk; ///< true if data exist on disk and can be loaded
	QString mFilename;
	vtkImageDataPtr mImageData;
};
typedef boost::shared_ptr<CachedImageData> CachedImageDataPtr;

/** Container inteface for managing a list of vtkImageData.
  * Subclasses implement storage of vtkImageData in various ways.
 *
 * \ingroup cxResourceUtilities
 * \date Dec 04 2012
 * \author christiana
 */
class ImageDataContainer
{
public:
	virtual ~ImageDataContainer() {}
	virtual vtkImageDataPtr get(unsigned index) = 0;
	virtual unsigned size() const = 0;
	bool empty() const { return this->size()==0; }
};
typedef boost::shared_ptr<ImageDataContainer> ImageDataContainerPtr;

/** Container class for delayed loading of images.
 *
 * \ingroup cxResourceUtilities
 * \date Dec 04 2012
 * \author christiana
 */
class CachedImageDataContainer : public ImageDataContainer
{
public:
	CachedImageDataContainer(QString baseFilename, int size);
	CachedImageDataContainer(std::vector<CachedImageDataPtr> frames);
	virtual ~CachedImageDataContainer() {}
	virtual vtkImageDataPtr get(unsigned index);
	virtual unsigned size() const;
private:
	std::vector<CachedImageDataPtr> mImages;
};

/** Container class for extracting 2D vtkImageData from a 3D base image.
 *
 * \ingroup cxResourceUtilities
 * \date Dec 04 2012
 * \author christiana
 */
class SplitFramesContainer : public ImageDataContainer
{
public:
	SplitFramesContainer(vtkImageDataPtr image3D);
	virtual ~SplitFramesContainer() {}
	virtual vtkImageDataPtr get(unsigned index);
	virtual unsigned size() const;
private:
	std::vector<vtkImageDataPtr> mImages;
	vtkImageDataPtr mOptionalWholeBase; ///< handle for original monolithic data if present
};


/**
 * \}
 */

} // namespace cx



#endif // CXIMAGEDATACONTAINER_H

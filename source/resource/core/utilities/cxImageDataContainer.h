/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIMAGEDATACONTAINER_H
#define CXIMAGEDATACONTAINER_H

#include "cxResourceExport.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include <vector>

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

/** Delayed loading of one vtkImageData.
 *
 *
 * \date Dec 04 2012
 * \author christiana
 */
class cxResource_EXPORT CachedImageData
{
public:
	/**
	  * Initialize with a given image file
	  */
	explicit CachedImageData(QString filename, vtkImageDataPtr image = NULL);
	~CachedImageData();
	/**
	  * Return name of file.
	  */
	QString getFilename() { return mFilename; }
	vtkImageDataPtr getImage();
	/**
	  * Clear the image contents from memory, if possible.
	  * Return true if purge was successful.
	  */
	bool purge();

private:
	QString mFilename;
	vtkImageDataPtr mImageData;
};
typedef boost::shared_ptr<CachedImageData> CachedImageDataPtr;

/** Container inteface for managing a list of vtkImageData.
  * Subclasses implement storage of vtkImageData in various ways.
 *
 * \date Dec 04 2012
 * \author christiana
 */
class cxResource_EXPORT ImageDataContainer
{
public:
	virtual ~ImageDataContainer() {}
	virtual vtkImageDataPtr get(unsigned index) = 0;
	virtual unsigned size() const = 0;
	bool empty() const { return this->size()==0; }
	virtual bool purge(unsigned index) { return false; }
	virtual void purgeAll();
};
typedef boost::shared_ptr<ImageDataContainer> ImageDataContainerPtr;

/** Container class for delayed loading of images.
  * The images will also not be stored by this container.
 *
 * \date Dec 04 2012
 * \author christiana
 */
class cxResource_EXPORT CachedImageDataContainer : public ImageDataContainer
{
public:
	CachedImageDataContainer();
	CachedImageDataContainer(QString baseFilename, int size);
	CachedImageDataContainer(std::vector<QString> frames);
//	CachedImageDataContainer(std::vector<CachedImageDataPtr> frames);
	virtual ~CachedImageDataContainer();
	virtual vtkImageDataPtr get(unsigned index);
	virtual QString getFilename(unsigned index);
	virtual unsigned size() const;
	virtual bool purge(unsigned index) { return mImages[index]->purge(); }
	/**
	* If set, image files managed by this object will be deleted when
	* object goes out of scope
	*/
	void setDeleteFilesOnRelease(bool on) { mDeleteFilesOnRelease = on; }
	void append(QString filename);
private:
	std::vector<CachedImageDataPtr> mImages;
	bool mDeleteFilesOnRelease;
};
typedef boost::shared_ptr<CachedImageDataContainer> CachedImageDataContainerPtr;

/** Container class for extracting 2D vtkImageData from a 3D base image.
 *
 * \date Dec 04 2012
 * \author christiana
 */
class cxResource_EXPORT SplitFramesContainer : public ImageDataContainer
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

/** Primitive implementation of ImageDataContainer interface,
 *  contains a list of image planes.
 *
 * \date Nov 28 2013
 * \author christiana
 */
class cxResource_EXPORT FramesDataContainer : public ImageDataContainer
{
public:
	FramesDataContainer(std::vector<vtkImageDataPtr> images);
	virtual ~FramesDataContainer() {}
	virtual vtkImageDataPtr get(unsigned index);
	virtual unsigned size() const;
private:
	std::vector<vtkImageDataPtr> mImages;
};


/**
 * \}
 */

} // namespace cx



#endif // CXIMAGEDATACONTAINER_H

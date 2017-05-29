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
	vtkImageDataPtr getImage(FileManagerServicePtr filemanager);
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
	CachedImageDataContainer(FileManagerServicePtr filemanagerservice);
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
	FileManagerServicePtr mFileManagerService;
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

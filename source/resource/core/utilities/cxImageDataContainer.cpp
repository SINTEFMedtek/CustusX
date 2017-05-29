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

#include "cxImageDataContainer.h"
#include <QDir>
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include "cxFileManagerService.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxUtilHelpers.h"
#include "cxBoundingBox3D.h"

typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

namespace cx
{

CachedImageData::CachedImageData(QString filename, vtkImageDataPtr image)
{
	mFilename = filename;
	mImageData = image;
}

CachedImageData::~CachedImageData()
{
}



vtkImageDataPtr CachedImageData::getImage(FileManagerServicePtr filemanager)
{
	if (!mImageData)
	{
		//mImageData = MetaImageReader().loadVtkImageData(mFilename);
		mImageData = filemanager->loadVtkImageData(mFilename);
	}
	return mImageData;
}

bool CachedImageData::purge()
{
		mImageData = vtkImageDataPtr();
		return true;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

void ImageDataContainer::purgeAll()
{
	int count = 0;
	for (unsigned i=0; i<this->size(); ++i)
	{
		if (this->purge(i))
			count++;
	}
//	double fraction = double(count)/this->size();
//	std::cout << QString("PurgeAll imagecache: purged %1\% of %2 frames.").arg(fraction).arg(this->size()) << std::endl;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


CachedImageDataContainer::CachedImageDataContainer(QString baseFilename, int size) :
    mDeleteFilesOnRelease(false)
{
	QFileInfo info(baseFilename);

	for (int i=0; true; ++i)
	{
		QString file = info.absolutePath()+"/"+info.completeBaseName()+QString("_%1.mhd").arg(i);

		if (!QFileInfo(file).exists())
			break;

		CachedImageDataPtr cache(new CachedImageData(file));
		mImages.push_back(cache);

		if (size>=0 && i>=size)
			break;
	}

	if (size>=0)
	{
		CX_ASSERT(size == mImages.size());
	}
}

CachedImageDataContainer::CachedImageDataContainer(FileManagerServicePtr filemanagerservice) :
	mDeleteFilesOnRelease(false),
	mFileManagerService(filemanagerservice)
{
}

CachedImageDataContainer::CachedImageDataContainer(std::vector<QString> frames) :
    mDeleteFilesOnRelease(false)
{
	for (unsigned i=0; i<frames.size(); ++i)
		this->append(frames[i]);
}

void CachedImageDataContainer::append(QString filename)
{
	mImages.push_back(CachedImageDataPtr(new CachedImageData(filename)));
}

CachedImageDataContainer::~CachedImageDataContainer()
{
	if (mDeleteFilesOnRelease)
	{
		for (unsigned i=0; i<mImages.size(); ++i)
		{
			QDir().remove(mImages[i]->getFilename());
			QDir().remove(changeExtension(mImages[i]->getFilename(), "raw"));
			QDir().remove(changeExtension(mImages[i]->getFilename(), "zraw"));
		}
	}
}

vtkImageDataPtr CachedImageDataContainer::get(unsigned index)
{
	CX_ASSERT(index < this->size());
	CX_ASSERT(mImages[index]);

	if (index >= this->size())
	{
		std::cout << QString("Attempt to call index %1, size=%2").arg(index).arg(this->size()) << std::endl;
	}
//	int* a = NULL;
//	*a = 5;
	vtkImageDataPtr retval = mImages[index]->getImage(mFileManagerService);
	mImages[index]->purge();
	return retval;
}

QString CachedImageDataContainer::getFilename(unsigned index)
{
	return mImages[index]->getFilename();
}


unsigned CachedImageDataContainer::size() const
{
	return (unsigned)mImages.size();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

SplitFramesContainer::SplitFramesContainer(vtkImageDataPtr image3D)
{
	mOptionalWholeBase = image3D;
	mImages.resize(image3D->GetDimensions()[2]);

	for (int i=0; i<mImages.size(); ++i)
	{
		vtkImageImportPtr import = vtkImageImportPtr::New();

		import->SetImportVoidPointer(image3D->GetScalarPointer(0,0,i));
		import->SetDataScalarType(image3D->GetScalarType());
		import->SetDataSpacing(image3D->GetSpacing());
		import->SetNumberOfScalarComponents(image3D->GetNumberOfScalarComponents());
//		int* extent = image3D->GetWholeExtent();
//		int* extent = image3D->GetExtent();
		IntBoundingBox3D extent(image3D->GetExtent());
		extent[4] = 0;
		extent[5] = 0;
		import->SetWholeExtent(extent.data());
		import->SetDataExtentToWholeExtent();

		import->Update();
		mImages[i] = import->GetOutput();

//		std::cout << "SplitFramesContainer " << i << std::endl;
//		std::cout << "===================================" << std::endl;
//		mImages[i]->Print(std::cout);
	}
}

vtkImageDataPtr SplitFramesContainer::get(unsigned index)
{
	return mImages[index];
}

unsigned SplitFramesContainer::size() const
{
	return (unsigned)mImages.size();
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

FramesDataContainer::FramesDataContainer(std::vector<vtkImageDataPtr> images)
{
	mImages = images;
}

vtkImageDataPtr FramesDataContainer::get(unsigned index)
{
	return mImages[index];
}

unsigned FramesDataContainer::size() const
{
	return mImages.size();
}

} // namespace cx

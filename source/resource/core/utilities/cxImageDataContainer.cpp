/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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


CachedImageDataContainer::CachedImageDataContainer(QString baseFilename, int size, FileManagerServicePtr filemanagerservice) :
	mDeleteFilesOnRelease(false),
	mFileManagerService(filemanagerservice)
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

CachedImageDataContainer::CachedImageDataContainer(std::vector<QString> frames, FileManagerServicePtr filemanagerservice) :
	mDeleteFilesOnRelease(false),
	mFileManagerService(filemanagerservice)
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

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

#include "cxImageDataContainer.h"
#include <QDir>
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include "sscDataReaderWriter.h"
#include "sscLogger.h"
#include "sscTypeConversions.h"
#include "sscUtilHelpers.h"

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



vtkImageDataPtr CachedImageData::getImage()
{
	if (!mImageData)
	{
		mImageData = ssc::MetaImageReader().loadVtkImageData(mFilename);
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
	double fraction = double(count)/this->size();
//	std::cout << QString("PurgeAll imagecache: purged %1\% of %2 frames.").arg(fraction).arg(this->size()) << std::endl;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


CachedImageDataContainer::CachedImageDataContainer(QString baseFilename, int size) :
    mDeleteFilesOnRelease(false)
{
	QFileInfo info(baseFilename);

	for (unsigned i=0; true; ++i)
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
		SSC_ASSERT(size == mImages.size());
	}
}

CachedImageDataContainer::CachedImageDataContainer() :
    mDeleteFilesOnRelease(false)
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
			QDir().remove(ssc::changeExtension(mImages[i]->getFilename(), "raw"));
			QDir().remove(ssc::changeExtension(mImages[i]->getFilename(), "zraw"));
		}
	}
}

vtkImageDataPtr CachedImageDataContainer::get(unsigned index)
{
	SSC_ASSERT(index < this->size());
	SSC_ASSERT(mImages[index]);

	if (index >= this->size())
	{
		std::cout << QString("Attempt to call index %1, size=%2").arg(index).arg(this->size()) << std::endl;
	}
//	int* a = NULL;
//	*a = 5;
	vtkImageDataPtr retval = mImages[index]->getImage();
	mImages[index]->purge();
	return retval;
}

QString CachedImageDataContainer::getFilename(unsigned index)
{
	return mImages[index]->getFilename();
}


unsigned CachedImageDataContainer::size() const
{
	return mImages.size();
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
		int* extent = image3D->GetWholeExtent();
		extent[4] = 0;
		extent[5] = 0;
		import->SetWholeExtent(extent);
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
	return mImages.size();
}


} // namespace cx

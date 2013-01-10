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
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include "sscDataManagerImpl.h"
#include "sscLogger.h"

typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

namespace cx
{

CachedImageData::CachedImageData(QString filename, vtkImageDataPtr image)
{
	mPurgeValid = false;
	mFilename = filename;
	mImageData = image;
}

vtkImageDataPtr CachedImageData::getImage()
{
// no cache: careful - leads to several loads and multiple storage outside - more mem use
//	return ssc::MetaImageReader().load(mFilename);

	if (!mImageData)
	{
		mImageData = ssc::MetaImageReader().load(mFilename);
	}
	return mImageData;
}

void CachedImageData::setPurgeValid()
{
	mPurgeValid = true;
}

bool CachedImageData::purge()
{
	if (mPurgeValid)
		return false;

	mImageData = vtkImageDataPtr();
	return false;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


CachedImageDataContainer::CachedImageDataContainer(QString baseFilename, int size)
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

CachedImageDataContainer::CachedImageDataContainer(std::vector<CachedImageDataPtr> frames)
{
	mImages = frames;
}


vtkImageDataPtr CachedImageDataContainer::get(unsigned index)
{
	return mImages[index]->getImage();
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

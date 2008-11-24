#include "sscDataManager.h"

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkMetaImageReader> vtkMetaImageReaderPtr;
#include <vtkMetaImageReader.h>
#include <vtkImageData.h>

namespace ssc
{

ImagePtr MetaImageReader::load(const std::string& filename)
{
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(filename.c_str());
	reader->ReleaseDataFlagOn();
	reader->GetOutput()->ReleaseDataFlagOn();
	reader->Update();
	vtkImageDataPtr imageData = reader->GetOutput();

//		ImagePtr image(new Image());
//		image->setVtkImageData(imageData);
//		return image;
	return ImagePtr(new Image(filename, imageData));

}

// --------------------------------------------------------
DataManager* DataManager::mInstance = NULL; ///< static member
// --------------------------------------------------------

DataManager* DataManager::instance()
{
	if (!mInstance)
	{
		mInstance = new DataManager();
	}
	return mInstance;
}


DataManager::DataManager()
{
	mImageReaders[rtMETAIMAGE].reset(new MetaImageReader());
}

DataManager::~DataManager()
{
}

ImagePtr DataManager::loadImage(const std::string& filename, READER_TYPE type)
{
	if (type==rtAUTO)
	{
		// do special stuff
		return ImagePtr();
	}

	// identify type
	ImagePtr current = mImageReaders[type]->load(filename);
	if (current)
	{
		mImages[current->getUid()] = current;
	}
	return current;
}


} // namespace ssc

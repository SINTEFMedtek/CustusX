#include "sscDataManager.h"

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkMetaImageReader> vtkMetaImageReaderPtr;

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
typedef vtkSmartPointer<class vtkPolyDataReader> vtkPolyDataReaderPtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;


namespace ssc
{

//-----
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

//-----
MeshPtr PolyDataMeshReader::load(const std::string& fileName)
{
	vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
	reader->SetFileName(fileName.c_str());
	reader->Update();
	vtkPolyDataPtr polyData = reader->GetOutput();

	//return MeshPtr(new Mesh(fileName, fileName, polyData));
	return MeshPtr(new Mesh(fileName, "PolyData", polyData));

}

MeshPtr StlMeshReader::load(const std::string& fileName)
{
	vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
	reader->SetFileName(fileName.c_str());
	reader->Update();
	vtkPolyDataPtr polyData = reader->GetOutput();

	//return MeshPtr(new Mesh(fileName, fileName, polyData));
	return MeshPtr(new Mesh(fileName, "PolyData", polyData));

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
	
	mMeshReaders[mrtPOLYDATA].reset(new PolyDataMeshReader());
	mMeshReaders[mrtSTL].reset(new StlMeshReader());
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

// meshes
MeshPtr DataManager::loadMesh(const std::string& fileName, MESH_READER_TYPE meshType)
{
	// identify type
	MeshPtr newMesh = mMeshReaders[meshType]->load(fileName);
	if (newMesh)
	{
		mMeshes[newMesh->getUID()] = newMesh;
	}
	return newMesh;
}


} // namespace ssc

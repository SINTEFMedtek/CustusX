#include "sscDataManagerImpl.h"

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkMetaImageReader> vtkMetaImageReaderPtr;

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
typedef vtkSmartPointer<class vtkPolyDataReader> vtkPolyDataReaderPtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;

#include <QDomDocument>

namespace ssc
{

//-----
ImagePtr MetaImageReader::load(const std::string& filename)
{
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(filename.c_str());
	reader->ReleaseDataFlagOn();
	//reader->GetOutput()->ReleaseDataFlagOn();
	reader->Update();
	vtkImageDataPtr imageData = reader->GetOutput();
  
  ImagePtr image(new Image(filename, imageData));
  return image;
  
}

//-----
MeshPtr PolyDataMeshReader::load(const std::string& fileName)
{
	vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
	reader->SetFileName(fileName.c_str());
	reader->Update();
	vtkPolyDataPtr polyData = reader->GetOutput();

	//return MeshPtr(new Mesh(fileName, fileName, polyData));
  MeshPtr tempMesh(new Mesh(fileName, "PolyData", polyData));
  return tempMesh;

}

MeshPtr StlMeshReader::load(const std::string& fileName)
{
	vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
	reader->SetFileName(fileName.c_str());
	reader->Update();
	vtkPolyDataPtr polyData = reader->GetOutput();

	//return MeshPtr(new Mesh(fileName, fileName, polyData));
  MeshPtr tempMesh(new Mesh(fileName, "PolyData", polyData));
  return tempMesh;

}

void DataManagerImpl::initialize()
{
	setInstance(new DataManagerImpl());
}

DataManagerImpl::DataManagerImpl()
{
	mImageReaders[rtMETAIMAGE].reset(new MetaImageReader());
	mMeshReaders[mrtPOLYDATA].reset(new PolyDataMeshReader());
	mMeshReaders[mrtSTL].reset(new StlMeshReader());
	mCenter = Vector3D(0,0,0);
}

DataManagerImpl::~DataManagerImpl()
{
}

Vector3D DataManagerImpl::getCenter() const
{
	return mCenter;
}
void DataManagerImpl::setCenter(const Vector3D& center)
{
	mCenter = center;
	emit centerChanged();
}

ImagePtr DataManagerImpl::loadImage(const std::string& filename, READER_TYPE type)
{
	if (mImages.count(filename)) // dont load same image twice
	{
		return mImages[filename];
	}	
	
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
		emit dataLoaded();
	}
	return current;
}

// meshes
MeshPtr DataManagerImpl::loadMesh(const std::string& fileName, MESH_READER_TYPE meshType)
{
	// identify type
	MeshPtr newMesh = mMeshReaders[meshType]->load(fileName);
	if (newMesh)
	{
		mMeshes[newMesh->getUID()] = newMesh;
		emit dataLoaded();
	}
	return newMesh;
}

ImagePtr DataManagerImpl::getImage(const std::string& uid)
{
	if (!mImages.count(uid))
		return ImagePtr();
	return mImages[uid];
}

std::map<std::string, ImagePtr> DataManagerImpl::getImages()
{
	return mImages;
}

std::map<std::string, std::string> DataManagerImpl::getImageUidsAndNames() const
{
	std::map<std::string, std::string> retval;
	for (ImagesMap::const_iterator iter=mImages.begin(); iter!=mImages.end(); ++iter)
		retval[iter->first] = iter->second->getName();
	return retval;
}

std::vector<std::string> DataManagerImpl::getImageNames() const
{
	std::vector<std::string> retval;
	for (ImagesMap::const_iterator iter=mImages.begin(); iter!=mImages.end(); ++iter)
		retval.push_back(iter->second->getName());
	return retval;
}

std::vector<std::string> DataManagerImpl::getImageUids() const
{
	std::vector<std::string> retval;
	for (ImagesMap::const_iterator iter=mImages.begin(); iter!=mImages.end(); ++iter)
		retval.push_back(iter->first);
	return retval;
}

MeshPtr DataManagerImpl::getMesh(const std::string& uid) { return MeshPtr(); }
std::map<std::string, MeshPtr> DataManagerImpl::getMeshes() { return std::map<std::string, MeshPtr>();  }
std::map<std::string, std::string> DataManagerImpl::getMeshUIDsWithNames() const { return std::map<std::string, std::string>(); }
std::vector<std::string> DataManagerImpl::getMeshUIDs() const {  return std::vector<std::string>(); }
std::vector<std::string> DataManagerImpl::getMeshNames() const {  return std::vector<std::string>(); }

void DataManagerImpl::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement dataManagerNode = doc.createElement("datamanager");
  parentNode.appendChild(dataManagerNode);

  for(ImagesMap::const_iterator iter=mImages.begin(); iter!=mImages.end(); ++iter)
  {
    iter->second->addXml(dataManagerNode);
  }
}
void DataManagerImpl::parseXml(QDomNode& dataNode)
{
  // All images must be created from the DataManager, so the image nodes
  // are parsed here
  QDomNode node = dataNode.firstChild();
  while (!node.isNull())
  {
    if (node.nodeName() == "image")
    {
      QDomElement uidNode = node.namedItem("uid").toElement();
      QDomElement nameNode = node.namedItem("name").toElement();
      if (!uidNode.isNull())
      {
        ssc::ImagePtr image = loadImage(uidNode.text().toStdString(),
                                        ssc::rtMETAIMAGE);
        image->setName(nameNode.text().toStdString());
        image->parseXml(node);
      }
      else
      {
        std::cout << "Warning: DataManager::parseXml() found no uid for image";
        std::cout << std::endl;
      }
    }
    else
    {
      std::cout << "Warning: DataManager::parseXml() found unknown XML node: ";
      std::cout << node.nodeName().toStdString() << std::endl;
    }
    node = node.nextSibling();
  }
}

} // namespace ssc


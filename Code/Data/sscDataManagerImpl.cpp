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
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "sscTransform3D.h"

namespace ssc
{

//-----
ImagePtr MetaImageReader::load(const std::string& filename)
{
  //read the specific TransformMatrix-tag from the header
  Transform3DPtr rMd(new Transform3D);
  QFile file(filename.c_str());
  QString line;
  if(file.open(QIODevice::ReadOnly))
  {
    QTextStream t(&file);
    while(!t.atEnd())
    {
      line.clear();
      line = t.readLine();
      // do something with the line
      if(line.startsWith("Position",Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        (*rMd)[0][3] = list.at(2).toDouble();
        (*rMd)[1][3] = list.at(3).toDouble();
        (*rMd)[2][3] = list.at(4).toDouble();
      }
      else if(line.startsWith("TransformMatrix",Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        (*rMd)[0][0] = list.at(2).toDouble();
        (*rMd)[0][1] = list.at(3).toDouble();
        (*rMd)[0][2] = list.at(4).toDouble();
        (*rMd)[1][0] = list.at(5).toDouble();
        (*rMd)[1][1] = list.at(6).toDouble();
        (*rMd)[1][2] = list.at(7).toDouble();
        (*rMd)[2][0] = list.at(8).toDouble();
        (*rMd)[2][1] = list.at(9).toDouble();
        (*rMd)[2][2] = list.at(10).toDouble();
      }
    }
    file.close();
  }

  //load the image from file
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(filename.c_str());
	reader->ReleaseDataFlagOn();
	//reader->GetOutput()->ReleaseDataFlagOn();
	reader->Update();
	vtkImageDataPtr imageData = reader->GetOutput();
  
  ImagePtr image(new Image(filename, imageData));
  image->set_rMd((*rMd));
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
		mMeshes[newMesh->getUid()] = newMesh;
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
std::map<std::string, std::string> DataManagerImpl::getMeshUidsWithNames() const { return std::map<std::string, std::string>(); }
std::vector<std::string> DataManagerImpl::getMeshUids() const {  return std::vector<std::string>(); }
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
  for(std::map<std::string, MeshPtr>::const_iterator iter=mMeshes.begin(); iter!=mMeshes.end(); ++iter)
  {
    iter->second->addXml(dataManagerNode);
  }
}
void DataManagerImpl::parseXml(QDomNode& dataNode, QString absolutePath)
{
  // All images must be created from the DataManager, so the image nodes
  // are parsed here
  QDomNode node = dataNode.firstChild();
  QDomElement nameNode;
  QString path;//Path to data file
  
  while (!node.isNull())
  {
    if(node.nodeName() == "image" || node.nodeName() == "mesh")
    {
      QDomElement uidNode = node.namedItem("uid").toElement();
      nameNode = node.namedItem("name").toElement();
      QDomElement filePathNode = node.namedItem("filePath").toElement();
      if(!filePathNode.isNull()) 
      {
        path = filePathNode.text();
        QDir relativePath = QDir(QString(path));
        if(!absolutePath.isEmpty())
          if(relativePath.isRelative())
            path = absolutePath+"/"+path;
          else //Make relative
          {
            QDir patientDataDir(absolutePath);
            relativePath.setPath(patientDataDir.relativeFilePath(relativePath.path()));
          }
        
        if(!path.isEmpty())
        {
          ssc::DataPtr data;
          
          QFileInfo fileInfo(path);
          QString fileType = fileInfo.suffix();
          if(fileType.compare("mhd", Qt::CaseInsensitive) == 0 ||
             fileType.compare("mha", Qt::CaseInsensitive) == 0)
          {
            data = this->loadImage(path.toStdString(), ssc::rtMETAIMAGE);
          }
          else if(fileType.compare("stl", Qt::CaseInsensitive) == 0)
          {
            data = this->loadMesh(path.toStdString(), ssc::mrtSTL);
          }
          else if(fileType.compare("vtk", Qt::CaseInsensitive) == 0)
          {
            data = this->loadMesh(path.toStdString(), ssc::mrtPOLYDATA);
          }
          
          data->setName(nameNode.text().toStdString());
          data->setFilePath(relativePath.path().toStdString());
          data->parseXml(node);
        }
        else
        {
          std::cout << "Warning: DataManager::parseXml() empty filePath for data";
          std::cout << std::endl;
        }
      }
      // Don't use uid as path
      //else if(!uidNode.isNull())
      //  path = uidNode.text().toStdString();
      else
      {
        std::cout << "Warning: DataManager::parseXml() found no filePath for data";
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


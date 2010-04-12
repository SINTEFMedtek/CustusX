#include "sscDataManagerImpl.h"

#include <sstream>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkMetaImageReader> vtkMetaImageReaderPtr;

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkImageChangeInformation.h>
typedef vtkSmartPointer<class vtkPolyDataReader> vtkPolyDataReaderPtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;
typedef vtkSmartPointer<class vtkImageChangeInformation> vtkImageChangeInformationPtr;


#include <QtCore>
#include <QDomDocument>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "sscTransform3D.h"
#include "sscRegistrationTransform.h"

namespace ssc
{

//-----
ImagePtr MetaImageReader::load(const std::string& filename)
{
  //read the specific TransformMatrix-tag from the header
  Vector3D p_r(0,0,0);
  Vector3D e_x(1,0,0);
  Vector3D e_y(0,1,0);
  Vector3D e_z(0,0,1);

  QString creator;
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
      if(line.startsWith("Position",Qt::CaseInsensitive)
          || line.startsWith("Offset",Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        p_r = Vector3D(list[2].toDouble(), list[3].toDouble(), list[4].toDouble());
      }
      else if(line.startsWith("TransformMatrix",Qt::CaseInsensitive)
          || line.startsWith("Orientation",Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);

        e_x = Vector3D(list[2].toDouble(), list[3].toDouble(), list[4].toDouble());
        e_y = Vector3D(list[5].toDouble(), list[6].toDouble(), list[7].toDouble());
        e_z = cross(e_x, e_y);
      }
      else if (line.startsWith("Creator",Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        creator = list[2];
      }
    }
    file.close();
  }

  // MDH is a volatile format: warn if we dont know the source
  if (( creator!="Ingerid" )&&( creator != "DICOMtoMetadataFilter" ))
  {
    std::cout << "WARNING: Loading file " + filename + ": unrecognized creator. Position/Orientation cannot be trusted" << std::endl;
  }

  Transform3D rMd;

  // add rotational part
  for (unsigned i=0; i<3; ++i)
  {
    rMd[i][0] = e_x[i];
    rMd[i][1] = e_y[i];
    rMd[i][2] = e_z[i];
  }

  // Special Ingerid Reinertsen fix: Position is stored as p_d instead of p_r: convert here
  if (creator=="Ingerid")
  {
    std::cout << "ingrid fixing" << std::endl;
    p_r = rMd.coord(p_r);
  }

  // add translational part
  rMd[0][3] = p_r[0];
  rMd[1][3] = p_r[1];
  rMd[2][3] = p_r[2];


  //load the image from file
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(filename.c_str());
	reader->ReleaseDataFlagOn();
	//reader->GetOutput()->ReleaseDataFlagOn();
	reader->Update();

	vtkImageChangeInformationPtr zeroer = vtkImageChangeInformationPtr::New();
	zeroer->SetInput(reader->GetOutput());
  zeroer->SetOutputOrigin(0,0,0);

  vtkImageDataPtr imageData = zeroer->GetOutput();
  imageData->Update();
  
  ImagePtr image(new Image(filename, imageData));

  RegistrationTransform regTrans(rMd, QFileInfo(file.fileName()).lastModified(), "From MHD file");
  image->get_rMd_History()->addRegistration(regTrans);

//  std::cout << "ImagePtr MetaImageReader::load \n" << *rMd << std::endl << std::endl;
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
	mActiveImage.reset();
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
ImagePtr DataManagerImpl::getActiveImage() const
{
  return mActiveImage;
}
void DataManagerImpl::setActiveImage(ImagePtr activeImage)
{
  if(mActiveImage == activeImage)
    return;

  mActiveImage = activeImage;

  std::string uid = "";
  if(mActiveImage)
    uid = mActiveImage->getUid();

  emit activeImageChanged(uid);
  std::cout << "Active image set to "<< uid << std::endl;
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

  QDomElement activeImageNode = doc.createElement("activeImage");
  if(mActiveImage)
    activeImageNode.appendChild(doc.createTextNode(mActiveImage->getUid().c_str()));
  dataManagerNode.appendChild(activeImageNode);

  //TODO
  /*QDomElement activeMeshNode = doc.createElement("activeMesh");
  if(mActiveMesh)
    activeMeshNode.appendChild(doc.createTextNode(mActiveMesh->getUid().c_str()));
  dataManagerNode.appendChild(activeMeshNode);*/

  QDomElement centerNode = doc.createElement("center");
  std::stringstream centerStream;
  centerStream << mCenter;
  centerNode.appendChild(doc.createTextNode(centerStream.str().c_str()));
  dataManagerNode.appendChild(centerNode);

  for(ImagesMap::const_iterator iter=mImages.begin(); iter!=mImages.end(); ++iter)
  {
    iter->second->addXml(dataManagerNode);
  }
  for(std::map<std::string, MeshPtr>::const_iterator iter=mMeshes.begin(); iter!=mMeshes.end(); ++iter)
  {
    iter->second->addXml(dataManagerNode);
  }
}
void DataManagerImpl::parseXml(QDomNode& dataManagerNode, QString absolutePath)
{
  // All images must be created from the DataManager, so the image nodes
  // are parsed here
  QDomNode child = dataManagerNode.firstChild();
  QDomElement nameNode;
  QString path;//Path to data file
  
  while (!child.isNull())
  {
    if(child.nodeName() == "image" || child.nodeName() == "mesh")
    {
      QDomElement uidNode = child.namedItem("uid").toElement();
      nameNode = child.namedItem("name").toElement();
      QDomElement filePathNode = child.namedItem("filePath").toElement();
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
          
          if(nameNode.text().isEmpty())
            data->setName(fileInfo.fileName().toStdString());
          else
            data->setName(nameNode.text().toStdString());
          data->setFilePath(relativePath.path().toStdString());
          data->parseXml(child);
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
      std::cout << child.nodeName().toStdString() << std::endl;
    }
    child = child.nextSibling();
  }

  //we need to make sure all images are loaded before we try to set an active image
  child = dataManagerNode.firstChild();
  while(!child.isNull())
  {
    if(child.toElement().tagName() == "activeImage")
    {
      const QString activeImageString = child.toElement().text();
      //std::cout << "Found a activeImage with uid: " << activeImageString.toStdString().c_str() << std::endl;
      if(!activeImageString.isEmpty())
      {
        ImagePtr image = this->getImage(activeImageString.toStdString());
        //std::cout << "Got an image with uid: " << image->getUid().c_str() << std::endl;
        this->setActiveImage(image);
      }
    }
    //TODO add activeMesh
    if(child.toElement().tagName() == "center")
    {
      const QString centerString = child.toElement().text();
      if(!centerString.isEmpty())
      {
        Vector3D center = Vector3D::fromString(centerString);
        this->setCenter(center);
        std::cout << "Center: " << center << std::endl;
      }
    }
    child = child.nextSibling();
  }
}

} // namespace ssc


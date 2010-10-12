#include "sscDataManagerImpl.h"

#include <sstream>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkMetaImageWriter.h>
typedef vtkSmartPointer<class vtkMetaImageReader> vtkMetaImageReaderPtr;
typedef vtkSmartPointer<class vtkMetaImageWriter> vtkMetaImageWriterPtr;

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
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscUtilHelpers.h"

namespace ssc
{

//-----
DataPtr MetaImageReader::load(const std::string& uid, const std::string& filename)
{
  //messageManager()->sendDebug("load filename: "+string_cast(filename));
  //read the specific TransformMatrix-tag from the header
  Vector3D p_r(0, 0, 0);
  Vector3D e_x(1, 0, 0);
  Vector3D e_y(0, 1, 0);
  Vector3D e_z(0, 0, 1);

  QString creator;
  QFile file(filename.c_str());

  QString line;
  if (file.open(QIODevice::ReadOnly))
  {
    QTextStream t(&file);
    while (!t.atEnd())
    {
      line.clear();
      line = t.readLine();
      // do something with the line
      if (line.startsWith("Position", Qt::CaseInsensitive) || line.startsWith("Offset", Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        p_r = Vector3D(list[2].toDouble(), list[3].toDouble(), list[4].toDouble());
      }
      else if (line.startsWith("TransformMatrix", Qt::CaseInsensitive) || line.startsWith("Orientation",
          Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);

        e_x = Vector3D(list[2].toDouble(), list[3].toDouble(), list[4].toDouble());
        e_y = Vector3D(list[5].toDouble(), list[6].toDouble(), list[7].toDouble());
        e_z = cross(e_x, e_y);
      }
      else if (line.startsWith("Creator", Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        creator = list[2];
      }
    }
    file.close();
  }

  // MDH is a volatile format: warn if we dont know the source
  if ((creator != "Ingerid") && (creator != "DICOMtoMetadataFilter"))
  {
    //std::cout << "WARNING: Loading file " + filename + ": unrecognized creator. Position/Orientation cannot be trusted" << std::endl;
  }

  Transform3D rMd;

  // add rotational part
  for (unsigned i = 0; i < 3; ++i)
  {
    rMd[i][0] = e_x[i];
    rMd[i][1] = e_y[i];
    rMd[i][2] = e_z[i];
  }

  // Special Ingerid Reinertsen fix: Position is stored as p_d instead of p_r: convert here
  if (creator == "Ingerid")
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
  zeroer->SetOutputOrigin(0, 0, 0);

  vtkImageDataPtr imageData = zeroer->GetOutput();
  imageData->Update();

  ImagePtr image(new Image(uid, imageData));

  RegistrationTransform regTrans(rMd, QFileInfo(file.fileName()).lastModified(), "From MHD file");
  image->get_rMd_History()->addRegistration(regTrans);

  //std::cout << "ImagePtr MetaImageReader::load" << std::endl << std::endl;
  return image;
}

//-----
DataPtr PolyDataMeshReader::load(const std::string& uid, const std::string& fileName)
{
  vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
  reader->SetFileName(fileName.c_str());
  reader->Update();
  vtkPolyDataPtr polyData = reader->GetOutput();

  //return MeshPtr(new Mesh(fileName, fileName, polyData));
  MeshPtr tempMesh(new Mesh(uid, "PolyData", polyData));
  return tempMesh;

}

DataPtr StlMeshReader::load(const std::string& uid, const std::string& fileName)
{
  vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
  reader->SetFileName(fileName.c_str());
  reader->Update();
  vtkPolyDataPtr polyData = reader->GetOutput();

  //return MeshPtr(new Mesh(fileName, fileName, polyData));
  MeshPtr tempMesh(new Mesh(uid, "PolyData", polyData));
  return tempMesh;

}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

void DataManagerImpl::initialize()
{
  setInstance(new DataManagerImpl());
}

DataManagerImpl::DataManagerImpl()
{
  mMedicalDomain = mdLABORATORY;
  //  mMedicalDomain = mdLAPAROSCOPY;
  mDataReaders[rtMETAIMAGE].reset(new MetaImageReader());
  mDataReaders[rtPOLYDATA].reset(new PolyDataMeshReader());
  mDataReaders[rtSTL].reset(new StlMeshReader());
  //	mCenter = Vector3D(0,0,0);
  //	mActiveImage.reset();
  this->clear();
}

DataManagerImpl::~DataManagerImpl()
{
}

void DataManagerImpl::clear()
{
  mData.clear();
  mCenter = ssc::Vector3D(0, 0, 0);
  //mMedicalDomain = mdLABORATORY; must be set explicitly
  //mMeshes.clear();
  mActiveImage.reset();
  mLandmarkProperties.clear();

  emit centerChanged();
  emit activeImageChanged("");
  emit activeImageTransferFunctionsChanged();
  emit landmarkPropertiesChanged();
  //emit medicalDomainChanged();
  emit dataLoaded();
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
  if (mActiveImage == activeImage)
    return;

  if (mActiveImage)
  {
    disconnect(mActiveImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
    //disconnect(mActiveImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    disconnect(mActiveImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
  }

  mActiveImage = activeImage;

  if (mActiveImage)
  {
    connect(mActiveImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(vtkImageDataChangedSlot()));
    //connect(mActiveImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
    connect(mActiveImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
  }
  this->vtkImageDataChangedSlot();
}

void DataManagerImpl::setLandmarkNames(std::vector<std::string> names)
{
  mLandmarkProperties.clear();
  for (unsigned i = 0; i < names.size(); ++i)
  {
    LandmarkProperty prop(string_cast(i + 1), names[i]); // generate 1-indexed uids (keep users happy)
    mLandmarkProperties[prop.getUid()] = prop;
  }
  emit landmarkPropertiesChanged();
}

std::string DataManagerImpl::addLandmark()
{
  int max = 0;
  std::map<std::string, LandmarkProperty>::iterator iter;
  for (iter = mLandmarkProperties.begin(); iter != mLandmarkProperties.end(); ++iter)
  {
    //max = std::max(max, qstring_cast(iter->second.getName()).toInt());
    max = std::max(max, qstring_cast(iter->first).toInt());
  }
  std::string uid = string_cast(max + 1);
  mLandmarkProperties[uid] = LandmarkProperty(uid);

  emit landmarkPropertiesChanged();
  return uid;
}

void DataManagerImpl::setLandmarkName(std::string uid, std::string name)
{
  mLandmarkProperties[uid].setName(name);
  emit landmarkPropertiesChanged();
}

std::map<std::string, LandmarkProperty> DataManagerImpl::getLandmarkProperties() const
{
  return mLandmarkProperties;
}

void DataManagerImpl::setLandmarkActive(std::string uid, bool active)
{
  mLandmarkProperties[uid].setActive(active);
  emit landmarkPropertiesChanged();
}

ImagePtr DataManagerImpl::loadImage(const std::string& uid, const std::string& filename, READER_TYPE type)
{
  this->loadData(uid,filename,type);
  return this->getImage(uid);
}

DataPtr DataManagerImpl::loadData(const std::string& uid, const std::string& path, READER_TYPE type)
{
  DataPtr data = this->readData(uid,path,type);
  this->loadData(data);
  return data;
}

/** Read a data set and return it. Do NOT add it to the datamanager.
 *  Internal method: used by loadData family.
 */
DataPtr DataManagerImpl::readData(const std::string& uid, const std::string& path, READER_TYPE type)
{
  QFileInfo fileInfo(qstring_cast(path));

  if (mData.count(uid)) // dont load same image twice
  {
    return mData[uid];
    //std::cout << "WARNING: Data with uid: " + uid + " already exists, abort loading.";
  }

  if (type == rtAUTO)
  {
    QString fileType = fileInfo.suffix();
    type = this->getReaderType(fileType);
  }

  // identify type
  DataPtr current = mDataReaders[type]->load(uid, path);

  if (!current)
    return DataPtr();

  current->setName(changeExtension(fileInfo.fileName(), "").toStdString());
  //data->setFilePath(relativePath.path().toStdString());

//  this->loadData(current);
  return current;
}


void DataManagerImpl::loadData(DataPtr data)
{
  if (data)
  {
    this->verifyParentFrame(data);
    mData[data->getUid()] = data;
    emit dataLoaded();
  }
}

void DataManagerImpl::saveImage(ImagePtr image, const std::string& basePath)
{
  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
  writer->SetInput(image->getBaseVtkImageData());
  writer->SetFileDimensionality(3);
  std::string filename = basePath + "/" + image->getFilePath();
  writer->SetFileName(filename.c_str());

  //Rename ending from .mhd to .raw
  QStringList splitName = qstring_cast(filename).split(".");
  splitName[splitName.size() - 1] = "raw";
  filename = string_cast(splitName.join("."));

  writer->SetRAWFileName(filename.c_str());
  writer->SetCompression(false);
  writer->Update();
  writer->Write();
}

// meshes
MeshPtr DataManagerImpl::loadMesh(const std::string& uid, const std::string& fileName, READER_TYPE type)
{
  this->loadData(uid,fileName,type);
  return this->getMesh(uid);
}

DataPtr DataManagerImpl::getData(const std::string& uid) const
{
  DataMap::const_iterator iter = mData.find(uid);
  if (iter==mData.end())
    return DataPtr();
  return iter->second;
}

void DataManagerImpl::verifyParentFrame(DataPtr data)
{
  if(data->getParentFrame().empty())
  {
    int max = 0;
    std::map<std::string, DataPtr>::iterator iter;
    for (iter = mData.begin(); iter != mData.end(); ++iter)
    {
      //max = std::max(max, qstring_cast(iter->first).toInt());
      QStringList parentList = qstring_cast(iter->second->getParentFrame()).split("_");
      if (parentList.size()<2)
        continue;
      max = std::max(max, parentList[1].toInt());
    }
    std::string parentFrame = "frame_" + string_cast(max + 1);
    data->setParentFrame(parentFrame);
  }
}

ImagePtr DataManagerImpl::getImage(const std::string& uid) const
{
  return boost::shared_dynamic_cast<Image>(this->getData(uid));
}

std::map<std::string, DataPtr> DataManagerImpl::getData() const
{
  return mData;
}

std::map<std::string, ImagePtr> DataManagerImpl::getImages() const
{
  std::map<std::string, ImagePtr> retval;
  for (DataMap::const_iterator iter=mData.begin(); iter!=mData.end(); ++iter)
  {
    ImagePtr image = this->getImage(iter->first);
    if (!image)
      continue;
    retval[iter->first] = image;
  }
  return retval;
}

std::map<std::string, std::string> DataManagerImpl::getImageUidsAndNames() const
{
  std::map<std::string, ImagePtr> images = this->getImages();

  std::map<std::string, std::string> retval;
  for (ImagesMap::const_iterator iter = images.begin(); iter != images.end(); ++iter)
    retval[iter->first] = iter->second->getName();
  return retval;
}

std::vector<std::string> DataManagerImpl::getImageNames() const
{
  std::map<std::string, ImagePtr> images = this->getImages();

  std::vector<std::string> retval;
  for (ImagesMap::const_iterator iter = images.begin(); iter != images.end(); ++iter)
    retval.push_back(iter->second->getName());
  return retval;
}

std::vector<std::string> DataManagerImpl::getImageUids() const
{
  std::map<std::string, ImagePtr> images = this->getImages();

  std::vector<std::string> retval;
  for (ImagesMap::const_iterator iter = images.begin(); iter != images.end(); ++iter)
    retval.push_back(iter->first);
  return retval;
}

MeshPtr DataManagerImpl::getMesh(const std::string& uid) const
{
  return boost::shared_dynamic_cast<Mesh>(this->getData(uid));
}

std::map<std::string, MeshPtr> DataManagerImpl::getMeshes() const
{
  std::map<std::string, MeshPtr> retval;
  for (DataMap::const_iterator iter=mData.begin(); iter!=mData.end(); ++iter)
  {
    MeshPtr mesh = this->getMesh(iter->first);
    if (!mesh)
      continue;
    retval[iter->first] = mesh;
  }
  return retval;
}

std::map<std::string, std::string> DataManagerImpl::getMeshUidsWithNames() const
{
  std::map<std::string, MeshPtr> meshes = getMeshes();

  std::map<std::string, std::string> retval;
  for (MeshMap::const_iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
    retval[iter->first] = iter->second->getName();
  return retval;
}
std::vector<std::string> DataManagerImpl::getMeshUids() const
{
  std::map<std::string, MeshPtr> meshes = this->getMeshes();

  std::vector<std::string> retval;
  for (MeshMap::const_iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
    retval.push_back(iter->first);
  return retval;
}

std::vector<std::string> DataManagerImpl::getMeshNames() const
{
  std::map<std::string, MeshPtr> meshes = this->getMeshes();

  std::vector<std::string> retval;
  for (MeshMap::const_iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
    retval.push_back(iter->second->getName());
  return retval;
}

void DataManagerImpl::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement dataManagerNode = doc.createElement("datamanager");
  parentNode.appendChild(dataManagerNode);

  QDomElement activeImageNode = doc.createElement("activeImageUid");
  if (mActiveImage)
    activeImageNode.appendChild(doc.createTextNode(mActiveImage->getUid().c_str()));
  dataManagerNode.appendChild(activeImageNode);

  QDomElement landmarkPropsNode = doc.createElement("landmarkprops");
  LandmarkPropertyMap::iterator it = mLandmarkProperties.begin();
  for (; it != mLandmarkProperties.end(); ++it)
  {
    QDomElement landmarkPropNode = doc.createElement("landmarkprop");
    it->second.addXml(landmarkPropNode);
    landmarkPropsNode.appendChild(landmarkPropNode);
  }
  dataManagerNode.appendChild(landmarkPropsNode);

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

  for (DataMap::const_iterator iter = mData.begin(); iter != mData.end(); ++iter)
  {
    QDomElement dataNode = doc.createElement("data");
    dataManagerNode.appendChild(dataNode);
    iter->second->addXml(dataNode);
  }
}

void DataManagerImpl::parseXml(QDomNode& dataManagerNode, QString rootPath)
{
  QDomNode landmarksNode = dataManagerNode.namedItem("landmarkprops");
  QDomElement landmarkNode = landmarksNode.firstChildElement("landmarkprop");
  for (; !landmarkNode.isNull(); landmarkNode = landmarkNode.nextSiblingElement("landmarkprop"))
  {
    LandmarkProperty landmarkProp;
    landmarkProp.parseXml(landmarkNode);
    mLandmarkProperties[landmarkProp.getUid()] = landmarkProp;
    //std::cout << "Loaded landmarkprop with name: " << landmarkProp.getName() << std::endl;
    emit landmarkPropertiesChanged();
  }

  // All images must be created from the DataManager, so the image nodes are parsed here
  QDomNode child = dataManagerNode.firstChild();
  for ( ; !child.isNull(); child = child.nextSibling())
  {
    if (child.nodeName() == "data")
    {
      this->loadData(child.toElement(), rootPath);
    }
  }

  //we need to make sure all images are loaded before we try to set an active image
  child = dataManagerNode.firstChild();
  while (!child.isNull())
  {
    if (child.toElement().tagName() == "activeImageUid")
    {
      const QString activeImageString = child.toElement().text();
      if (!activeImageString.isEmpty())
      {
        ImagePtr image = this->getImage(activeImageString.toStdString());
        this->setActiveImage(image);
      }
    }
    //TODO add activeMesh
    if (child.toElement().tagName() == "center")
    {
      const QString centerString = child.toElement().text();
      if (!centerString.isEmpty())
      {
        Vector3D center = Vector3D::fromString(centerString);
        this->setCenter(center);
      }
    }
    child = child.nextSibling();
  }
}

void DataManagerImpl::loadData(QDomElement node, QString rootPath)
{
  QString uidNodeString = node.namedItem("uid").toElement().text();

  QDomElement nameNode = node.namedItem("name").toElement();
  QDomElement filePathNode = node.namedItem("filePath").toElement();
  if (filePathNode.isNull())
  {
    messageManager()->sendWarning("Warning: DataManager::parseXml() found no filePath for data");
    return;
  }

  QString path = filePathNode.text();
  QDir relativePath = QDir(QString(path));
  if (!rootPath.isEmpty())
  {
    if (relativePath.isRelative())
      path = rootPath + "/" + path;
    else //Make relative
    {
      QDir patientDataDir(rootPath);
      relativePath.setPath(patientDataDir.relativeFilePath(relativePath.path()));
    }
  }

  if (path.isEmpty())
  {
    messageManager()->sendWarning("Warning: DataManager::parseXml() empty filePath for data");
    return;
  }

  ssc::DataPtr data = this->readData(string_cast(uidNodeString), string_cast(path), rtAUTO);

  if (!data)
  {
    messageManager()->sendWarning("Unknown file: " + string_cast(path));
    return;
  }

  if (!nameNode.text().isEmpty())
    data->setName(string_cast(nameNode.text()));
  data->setFilePath(relativePath.path().toStdString());
  data->parseXml(node);

  this->loadData(data);
}

READER_TYPE DataManagerImpl::getReaderType(QString fileType)
{
  if (fileType.compare("mhd", Qt::CaseInsensitive) == 0 || fileType.compare("mha", Qt::CaseInsensitive) == 0)
  {
    return ssc::rtMETAIMAGE;
  }
  else if (fileType.compare("stl", Qt::CaseInsensitive) == 0)
  {
    return ssc::rtSTL;
  }
  else if (fileType.compare("vtk", Qt::CaseInsensitive) == 0)
  {
    return ssc::rtPOLYDATA;
  }
  return ssc::rtCOUNT;
}

void DataManagerImpl::vtkImageDataChangedSlot()
{
  std::string uid = "";
  if (mActiveImage)
    uid = mActiveImage->getUid();

  emit activeImageChanged(uid);
  messageManager()->sendInfo("Active image set to " + string_cast(uid));
}

void DataManagerImpl::transferFunctionsChangedSlot()
{
  emit activeImageTransferFunctionsChanged();
}

MEDICAL_DOMAIN DataManagerImpl::getMedicalDomain() const
{
  return mMedicalDomain;
}

void DataManagerImpl::setMedicalDomain(MEDICAL_DOMAIN domain)
{
  if (mMedicalDomain==domain)
    return;
  mMedicalDomain = domain;
  emit medicalDomainChanged();
}

int DataManagerImpl::findUniqueUidNumber(std::string uidBase) const
{
  // Find an uid that is not used before
  int numMatches = 1;
  int recNumber = 0;

  if(numMatches != 0)
  {
    while(numMatches != 0)
    {
      QString newId = qstring_cast(uidBase).arg(++recNumber);
      numMatches = mData.count(string_cast(newId));
    }
  }
  return recNumber;
}

/** Create an image with unique uid. The input uidBase may contain %1 as a placeholder for a running integer that
 *  data manager can increment in order to obtain an unique uid. The same integer will be inserted into nameBase
 *  if %1 is found there
 *
 */
ImagePtr DataManagerImpl::createImage(vtkImageDataPtr data, std::string uidBase, std::string nameBase, std::string filePath)
{
  int recNumber = this->findUniqueUidNumber(uidBase);
  std::string uid = string_cast(qstring_cast(uidBase).arg(recNumber));
  std::string name = string_cast(qstring_cast(nameBase).arg(recNumber));
  ImagePtr retval = ImagePtr(new Image(uid, data, name));

  std::string filename = filePath + "/" + uid + ".mhd";
  retval->setFilePath(filename);

  return retval;
}
/** Create an image with unique uid. The input uidBase may contain %1 as a placeholder for a running integer that
 *  data manager can increment in order to obtain an unique uid. The same integer will be inserted into nameBase
 *  if %1 is found there
 *
 */
MeshPtr DataManagerImpl::createMesh(vtkPolyDataPtr data, std::string uidBase, std::string nameBase, std::string filePath)
{
  int recNumber = this->findUniqueUidNumber(uidBase);
  std::string uid = string_cast(qstring_cast(uidBase).arg(recNumber));
  std::string name = string_cast(qstring_cast(nameBase).arg(recNumber));
  MeshPtr retval = MeshPtr(new Mesh(uid, name, data));

  std::string filename = filePath + "/" + uid + ".vtk";
  retval->setFilePath(filename);

  return retval;
}


} // namespace ssc


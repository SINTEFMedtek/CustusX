#include "sscDataManagerImpl.h"

#include <sstream>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkMetaImageWriter.h>

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkSTLReader.h>
#include <vtkImageChangeInformation.h>
#include "vtkMINCImageReader.h"
#include "vtkTransform.h"
#include "vtkCommand.h"

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
#include "sscRTSource.h"
#include "sscCustomMetaImage.h"

namespace ssc
{



//-----
DataPtr MincImageReader::load(const QString& uid, const QString& filename)
{
  std::cout << "Reading " << filename << std::endl;

  //Read data input file
  vtkMINCImageReaderPtr l_dataReader = vtkMINCImageReaderPtr::New();
  l_dataReader->SetFileName(cstring_cast(filename));
  l_dataReader->Update();

  double l_dataOrigin[3];
  l_dataReader->GetOutput()->GetOrigin(l_dataOrigin);
  int l_dimensions[3];
  l_dataReader->GetOutput()->GetDimensions(l_dimensions);

  //set the transform
  vtkTransformPtr l_dataTransform = vtkTransformPtr::New();
  l_dataTransform->SetMatrix(l_dataReader->GetDirectionCosines());
  l_dataTransform->Translate(l_dataReader->GetDataOrigin());
  //l_dataTransform->GetInverse()->TransformPoint(l_dataOrigin, l_dataOrigin);
  //l_dataTransform->Translate(l_dataOrigin);
  //l_dataTransform->Scale(l_dataReader->GetOutput()->GetSpacing());

  ssc::Transform3D rMd(l_dataTransform->GetMatrix());

  // TODO: ensure rMd is correct in CustusX terms

  vtkImageChangeInformationPtr zeroer = vtkImageChangeInformationPtr::New();
  zeroer->SetInput(l_dataReader->GetOutput());
  zeroer->SetOutputOrigin(0, 0, 0);
  zeroer->Update();
//  vtkImageDataPtr imageData = zeroer->GetOutput();
  vtkImageDataPtr imageData = zeroer->GetOutput();


  QFile file(filename);
  QFileInfo info(file);
  //QString uid(info.completeBaseName()+"_minc_%1");
  QString name = uid;

  ImagePtr image(new Image(uid, imageData));
  //ssc::ImagePtr image = ssc::dataManager()->createImage(l_dataReader->GetOutput(),uid, name);
  image->get_rMd_History()->addRegistration(ssc::RegistrationTransform(rMd, info.lastModified(), "from Minc file"));
  image->getBaseVtkImageData()->Print(std::cout);

  return image;
    //////////////////////////////
}

class ErrorObserver : public vtkCommand
{
public:
  ErrorObserver() {}
  static ErrorObserver* New() {return new ErrorObserver;}
  virtual void Execute(vtkObject* caller, unsigned long, void* text)
  {
    mMessage = QString(reinterpret_cast<char*>(text));
    std::cout << "executing" << std::endl;
  }
  QString mMessage;


  static bool checkedRead(vtkSmartPointer<vtkAlgorithm> reader, QString filename)
  {
    vtkSmartPointer<ErrorObserver> errorObserver = vtkSmartPointer<ErrorObserver>::New();
    reader->AddObserver("ErrorEvent", errorObserver);

    reader->Update();

    if (!errorObserver->mMessage.isEmpty())
    {
      ssc::messageManager()->sendError("Load of data " + filename + " failed with message:\n" + errorObserver->mMessage);
      return false;
    }
    return true;
  }

};
namespace
{
}

//-----
DataPtr MetaImageReader::load(const QString& uid, const QString& filename)
{
  CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
  Transform3D rMd = customReader->readTransform();

  //load the image from file
  vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
  reader->SetFileName(cstring_cast(filename));
  reader->ReleaseDataFlagOn();

  if (!ErrorObserver::checkedRead(reader, filename))
    return DataPtr();

  vtkImageChangeInformationPtr zeroer = vtkImageChangeInformationPtr::New();
  zeroer->SetInput(reader->GetOutput());
  zeroer->SetOutputOrigin(0, 0, 0);

  vtkImageDataPtr imageData = zeroer->GetOutput();
  imageData->Update();

  ImagePtr image(new Image(uid, imageData));

  RegistrationTransform regTrans(rMd, QFileInfo(filename).lastModified(), "From MHD file");
  image->get_rMd_History()->addRegistration(regTrans);

  //std::cout << "ImagePtr MetaImageReader::load" << std::endl << std::endl;
  return image;
}

//-----
DataPtr PolyDataMeshReader::load(const QString& uid, const QString& fileName)
{
  vtkPolyDataReaderPtr reader = vtkPolyDataReaderPtr::New();
  reader->SetFileName(cstring_cast(fileName));

  if (!ErrorObserver::checkedRead(reader, fileName))
    return DataPtr();

  vtkPolyDataPtr polyData = reader->GetOutput();

  //return MeshPtr(new Mesh(fileName, fileName, polyData));
  MeshPtr tempMesh(new Mesh(uid, "PolyData", polyData));
  return tempMesh;

}


DataPtr StlMeshReader::load(const QString& uid, const QString& fileName)
{
  vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
  reader->SetFileName(cstring_cast(fileName));

  if (!ErrorObserver::checkedRead(reader, fileName))
    return DataPtr();

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
  mDataReaders[rtMINCIMAGE].reset(new MincImageReader());
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



// streams
RTSourcePtr DataManagerImpl::getStream(const QString& uid) const
{
  if (mStreams.count(uid))
    return mStreams.find(uid)->second;
  return RTSourcePtr();
}

DataManager::StreamMap DataManagerImpl::getStreams() const
{
  return mStreams;
}

void DataManagerImpl::loadStream(RTSourcePtr stream)
{
  if (!stream)
    return;
  mStreams[stream->getUid()] = stream;
  emit streamLoaded();
}

std::map<QString, RTSourcePtr> mStreams;


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

void DataManagerImpl::setLandmarkNames(std::vector<QString> names)
{
  mLandmarkProperties.clear();
  for (unsigned i = 0; i < names.size(); ++i)
  {
    LandmarkProperty prop(qstring_cast(i + 1), names[i]); // generate 1-indexed uids (keep users happy)
    mLandmarkProperties[prop.getUid()] = prop;
  }
  emit landmarkPropertiesChanged();
}

QString DataManagerImpl::addLandmark()
{
  int max = 0;
  std::map<QString, LandmarkProperty>::iterator iter;
  for (iter = mLandmarkProperties.begin(); iter != mLandmarkProperties.end(); ++iter)
  {
    //max = std::max(max, qstring_cast(iter->second.getName()).toInt());
    max = std::max(max, qstring_cast(iter->first).toInt());
  }
  QString uid = qstring_cast(max + 1);
  mLandmarkProperties[uid] = LandmarkProperty(uid);

  emit landmarkPropertiesChanged();
  return uid;
}

void DataManagerImpl::setLandmarkName(QString uid, QString name)
{
  mLandmarkProperties[uid].setName(name);
  emit landmarkPropertiesChanged();
}

std::map<QString, LandmarkProperty> DataManagerImpl::getLandmarkProperties() const
{
  return mLandmarkProperties;
}

void DataManagerImpl::setLandmarkActive(QString uid, bool active)
{
  mLandmarkProperties[uid].setActive(active);
  emit landmarkPropertiesChanged();
}

ImagePtr DataManagerImpl::loadImage(const QString& uid, const QString& filename, READER_TYPE type)
{
  this->loadData(uid,filename,type);
  return this->getImage(uid);
}

DataPtr DataManagerImpl::loadData(const QString& uid, const QString& path, READER_TYPE type)
{
  DataPtr data = this->readData(uid,path,type);
  this->loadData(data);
  return data;
}

/** Read a data set and return it. Do NOT add it to the datamanager.
 *  Internal method: used by loadData family.
 */
DataPtr DataManagerImpl::readData(const QString& uid, const QString& path, READER_TYPE type)
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

  if (!mDataReaders.count(type))
  {
    std::cout << "no reader found for file type " << path << ", " << uid << std::endl;
    return DataPtr();
  }

  // identify type
  DataPtr current = mDataReaders[type]->load(uid, path);

  if (!current)
    return DataPtr();

  current->setName(changeExtension(fileInfo.fileName(), ""));
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

//void DataManagerImpl::saveData(DataPtr data, const QString& basePath)
//{
//  ImagePtr image = boost::shared_dynamic_cast<Image>(data);
//  if (image)
//    this->saveImage(image, basePath);
//}

void DataManagerImpl::saveImage(ImagePtr image, const QString& basePath)
{
  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
  writer->SetInput(image->getBaseVtkImageData());
  writer->SetFileDimensionality(3);
  QString filename = basePath + "/" + image->getFilePath();
  writer->SetFileName(cstring_cast(filename));
  QDir().mkpath(QFileInfo(filename).path());

  //Rename ending from .mhd to .raw
  QStringList splitName = qstring_cast(filename).split(".");
  splitName[splitName.size() - 1] = "raw";
  QString rawfilename = splitName.join(".");

  writer->SetRAWFileName(cstring_cast(rawfilename));
  writer->SetCompression(false);
  writer->Update();
  writer->Write();

  writer = 0;

  CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
  customReader->setTransform(image->get_rMd());

//  QSettings header(filename, QSettings::IniFormat);
//  std::cout << "fn: " << header.fileName() << std::endl;
//  QStringList transform;
//  transform << "3" << "4" << "5";
//  header.setValue("TransformMatrix", transform);

}

// meshes
MeshPtr DataManagerImpl::loadMesh(const QString& uid, const QString& fileName, READER_TYPE type)
{
  this->loadData(uid,fileName,type);
  return this->getMesh(uid);
}

void DataManagerImpl::saveMesh(MeshPtr mesh, const QString& basePath)
{
  vtkPolyDataWriterPtr writer = vtkPolyDataWriterPtr::New();
  writer->SetInput(mesh->getVtkPolyData());
  //writer->SetFileDimensionality(3);
  QString filename = basePath + "/" + mesh->getFilePath();
  writer->SetFileName(cstring_cast(filename));

//  //Rename ending from .mhd to .raw
//  QStringList splitName = qstring_cast(filename).split(".");
//  splitName[splitName.size() - 1] = "raw";
//  filename = splitName.join(".");

  //writer->SetRAWFileName(filename.c_str());
  //writer->SetCompression(false);
  writer->Update();
  writer->Write();
}

DataPtr DataManagerImpl::getData(const QString& uid) const
{
  DataMap::const_iterator iter = mData.find(uid);
  if (iter==mData.end())
    return DataPtr();
  return iter->second;
}

void DataManagerImpl::verifyParentFrame(DataPtr data)
{
  if(data->getParentFrame().isEmpty())
  {
    int max = 0;
    std::map<QString, DataPtr>::iterator iter;
    for (iter = mData.begin(); iter != mData.end(); ++iter)
    {
      //max = std::max(max, qstring_cast(iter->first).toInt());
      QStringList parentList = qstring_cast(iter->second->getParentFrame()).split("_");
      if (parentList.size()<2)
        continue;
      max = std::max(max, parentList[1].toInt());
    }
    QString parentFrame = "frame_" + qstring_cast(max + 1);
    data->get_rMd_History()->addParentFrame(parentFrame);
  }
}

ImagePtr DataManagerImpl::getImage(const QString& uid) const
{
  return boost::shared_dynamic_cast<Image>(this->getData(uid));
}

std::map<QString, DataPtr> DataManagerImpl::getData() const
{
  return mData;
}

std::map<QString, ImagePtr> DataManagerImpl::getImages() const
{
  std::map<QString, ImagePtr> retval;
  for (DataMap::const_iterator iter=mData.begin(); iter!=mData.end(); ++iter)
  {
    ImagePtr image = this->getImage(iter->first);
    if (!image)
      continue;
    retval[iter->first] = image;
  }
  return retval;
}

std::map<QString, QString> DataManagerImpl::getImageUidsAndNames() const
{
  std::map<QString, ImagePtr> images = this->getImages();

  std::map<QString, QString> retval;
  for (ImagesMap::const_iterator iter = images.begin(); iter != images.end(); ++iter)
    retval[iter->first] = iter->second->getName();
  return retval;
}

std::vector<QString> DataManagerImpl::getImageNames() const
{
  std::map<QString, ImagePtr> images = this->getImages();

  std::vector<QString> retval;
  for (ImagesMap::const_iterator iter = images.begin(); iter != images.end(); ++iter)
    retval.push_back(iter->second->getName());
  return retval;
}

std::vector<QString> DataManagerImpl::getImageUids() const
{
  std::map<QString, ImagePtr> images = this->getImages();

  std::vector<QString> retval;
  for (ImagesMap::const_iterator iter = images.begin(); iter != images.end(); ++iter)
    retval.push_back(iter->first);
  return retval;
}

MeshPtr DataManagerImpl::getMesh(const QString& uid) const
{
  return boost::shared_dynamic_cast<Mesh>(this->getData(uid));
}

std::map<QString, MeshPtr> DataManagerImpl::getMeshes() const
{
  std::map<QString, MeshPtr> retval;
  for (DataMap::const_iterator iter=mData.begin(); iter!=mData.end(); ++iter)
  {
    MeshPtr mesh = this->getMesh(iter->first);
    if (!mesh)
      continue;
    retval[iter->first] = mesh;
  }
  return retval;
}

std::map<QString, QString> DataManagerImpl::getMeshUidsWithNames() const
{
  std::map<QString, MeshPtr> meshes = getMeshes();

  std::map<QString, QString> retval;
  for (MeshMap::const_iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
    retval[iter->first] = iter->second->getName();
  return retval;
}
std::vector<QString> DataManagerImpl::getMeshUids() const
{
  std::map<QString, MeshPtr> meshes = this->getMeshes();

  std::vector<QString> retval;
  for (MeshMap::const_iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
    retval.push_back(iter->first);
  return retval;
}

std::vector<QString> DataManagerImpl::getMeshNames() const
{
  std::map<QString, MeshPtr> meshes = this->getMeshes();

  std::vector<QString> retval;
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
    activeImageNode.appendChild(doc.createTextNode(mActiveImage->getUid()));
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
  centerNode.appendChild(doc.createTextNode(qstring_cast(mCenter)));
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
        ImagePtr image = this->getImage(activeImageString);
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

  ssc::DataPtr data = this->readData(uidNodeString, path, rtAUTO);

  if (!data)
  {
    messageManager()->sendWarning("Unknown file: " + path);
    return;
  }

  if (!nameNode.text().isEmpty())
    data->setName(nameNode.text());
  data->setFilePath(relativePath.path());
  data->parseXml(node);

  this->loadData(data);
}

READER_TYPE DataManagerImpl::getReaderType(QString fileType)
{
  if (fileType.compare("mhd", Qt::CaseInsensitive) == 0 || fileType.compare("mha", Qt::CaseInsensitive) == 0)
  {
    return ssc::rtMETAIMAGE;
  }
  else if (fileType.compare("mnc", Qt::CaseInsensitive) == 0)
  {
    return ssc::rtMINCIMAGE;
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
  QString uid = "";
  if (mActiveImage)
    uid = mActiveImage->getUid();

  emit activeImageChanged(uid);
  messageManager()->sendInfo("Active image set to " + qstring_cast(uid));
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

int DataManagerImpl::findUniqueUidNumber(QString uidBase) const
{
  if (!uidBase.contains("%"))
    return -1;
  // Find an uid that is not used before
  int numMatches = 1;
  int recNumber = 0;

  if(numMatches != 0)
  {
    while(numMatches != 0)
    {
      QString newId = qstring_cast(uidBase).arg(++recNumber);
      numMatches = mData.count(qstring_cast(newId));
    }
  }
  return recNumber;
}

/** Create an image with unique uid. The input uidBase may contain %1 as a placeholder for a running integer that
 *  data manager can increment in order to obtain an unique uid. The same integer will be inserted into nameBase
 *  if %1 is found there
 *
 */
ImagePtr DataManagerImpl::createImage(vtkImageDataPtr data, QString uidBase, QString nameBase, QString filePath)
{
  int recNumber = this->findUniqueUidNumber(uidBase);

  QString uid = qstring_cast(uidBase).arg(recNumber);
  QString name;

  if (recNumber==1)
    name = qstring_cast(nameBase).arg("");
  else
    name = qstring_cast(nameBase).arg("#%1").arg(recNumber);

  ImagePtr retval = ImagePtr(new Image(uid, data, name));

  QString filename = filePath + "/" + uid + ".mhd";
  retval->setFilePath(filename);

  return retval;
}
/** Create an image with unique uid. The input uidBase may contain %1 as a placeholder for a running integer that
 *  data manager can increment in order to obtain an unique uid. The same integer will be inserted into nameBase
 *  if %1 is found there
 *
 */
MeshPtr DataManagerImpl::createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath)
{
  int recNumber = this->findUniqueUidNumber(uidBase);
  QString uid = uidBase;
  QString name = nameBase;
  if (uid.contains("%"))
    uid = uidBase.arg(recNumber);
  if (name.contains("%"))
    name = nameBase.arg(recNumber);
  MeshPtr retval = MeshPtr(new Mesh(uid, name, data));

  QString filename = filePath + "/" + uid + ".vtk";
  retval->setFilePath(filename);

  return retval;
}

void DataManagerImpl::removeData(const QString& uid)
{
  if (this->getActiveImage() && this->getActiveImage()->getUid()==uid)
    this->setActiveImage(ImagePtr());

  mData.erase(uid);

  emit dataRemoved(uid);
  emit dataLoaded(); // this should alert everybody interested in the data as a collection.
}

} // namespace ssc


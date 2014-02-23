// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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
#include "sscVideoSource.h"
#include "sscCustomMetaImage.h"

#include "sscImageLUT2D.h"
#include "sscImageTF3D.h"
#include "sscLogger.h"
#include "sscDataReaderWriter.h"
#include "cxSpaceProvider.h"
#include "cxDataFactory.h"

namespace cx
{

void DataManagerImpl::initialize()
{
	setInstance(new DataManagerImpl());
}

DataManagerImpl::DataManagerImpl()
{
	mClinicalApplication = mdLABORATORY;
	m_rMpr_History.reset(new RegistrationHistory());
	connect(m_rMpr_History.get(), SIGNAL(currentChanged()), this, SIGNAL(rMprChanged()));
	mPatientLandmarks = Landmarks::create();
	this->clear();
}

DataManagerImpl::~DataManagerImpl()
{
}

void DataManagerImpl::setSpaceProvider(SpaceProviderPtr spaceProvider)
{
	mSpaceProvider = spaceProvider;
}

void DataManagerImpl::setDataFactory(DataFactoryPtr dataFactory)
{
	mDataFactory = dataFactory;
}

SpaceProviderPtr DataManagerImpl::getSpaceProvider()
{
	return mSpaceProvider;
}

DataFactoryPtr DataManagerImpl::getDataFactory()
{
	return mDataFactory;
}

void DataManagerImpl::clear()
{
	mData.clear();
	mCenter = Vector3D(0, 0, 0);
	mActiveImage.reset();
	mLandmarkProperties.clear();

	m_rMpr_History->clear();
	mPatientLandmarks->clear();

	emit centerChanged();
	emit activeImageChanged("");
	emit landmarkPropertiesChanged();
	emit dataAddedOrRemoved();
}

LandmarksPtr DataManagerImpl::getPatientLandmarks()
{
	return mPatientLandmarks;
}

// streams
VideoSourcePtr DataManagerImpl::getStream(const QString& uid) const
{
	if (mStreams.count(uid))
		return mStreams.find(uid)->second;
	return VideoSourcePtr();
}

DataManager::StreamMap DataManagerImpl::getStreams() const
{
	return mStreams;
}

void DataManagerImpl::loadStream(VideoSourcePtr stream)
{
	if (!stream)
		return;
	mStreams[stream->getUid()] = stream;
	emit streamLoaded();
}

std::map<QString, VideoSourcePtr> mStreams;

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

	mActiveImage = activeImage;

	QString uid = "";
	if (mActiveImage)
		uid = mActiveImage->getUid();

	emit activeImageChanged(uid);
	messageManager()->sendInfo("Active image set to " + qstring_cast(uid));
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

ImagePtr DataManagerImpl::loadImage(const QString& uid, const QString& filename)
{
	DataPtr data = this->loadData(uid, filename);
	if (!data)
		{
			messageManager()->sendError("Error with image file: " + filename);
			return ImagePtr();
		}
	return this->getImage(uid);
}

DataPtr DataManagerImpl::loadData(const QString& uid, const QString& path)
{
	if (mData.count(uid)) // dont load same image twice
		return mData[uid];

	QString type = DataReaderWriter().findDataTypeFromFile(path);
	DataPtr data = mDataFactory->create(type, uid);
	data->load(path);

//	DataPtr current = DataReaderWriter().readData(uid, path, type);
//	return current;

//	DataPtr data = this->readData(uid, path, "unknown");
	if (!data)
		{
			messageManager()->sendError("Error with data file: " + path);
			return DataPtr();
		}
	this->loadData(data);
	return data;
}

///** Read a data set and return it. Do NOT add it to the datamanager.
// *  Internal method: used by loadData family.
// */
//DataPtr DataManagerImpl::readData(const QString& uid, const QString& path, const QString& type)
//{
//	if (mData.count(uid)) // dont load same image twice
//	{
//		return mData[uid];
//	}

//	DataPtr current = mDataFactory->create(type, uid, name);
//	bool loaded = current->load(path);
//	if (loaded)
//		return current;

////	DataPtr current = DataReaderWriter().readData(uid, path, type);
////	return current;
//}

void DataManagerImpl::loadData(DataPtr data)
{
	if (data->getUid().contains('%'))
	{
		QString uid = data->getUid();
		QString name = data->getName();
		this->generateUidAndName(&uid, &name);
		data->setName(name);
		data->setUid(uid);
	}

	if (data)
	{
		this->verifyParentFrame(data);
		mData[data->getUid()] = data;
		emit dataAddedOrRemoved();
	}
}

void DataManagerImpl::saveImage(ImagePtr image, const QString& basePath)
{
	QString filename = basePath + "/Images/" + image->getUid() + ".mhd";
	image->setFilename(QDir(basePath).relativeFilePath(filename));

	MetaImageReader().saveImage(image, filename);
}

// meshes
MeshPtr DataManagerImpl::loadMesh(const QString& uid, const QString& fileName)
{
	DataPtr data = this->loadData(uid, fileName);
	if (!data)
		{
			messageManager()->sendError("Error with mesh file: " + fileName);
			return MeshPtr();
		}
	return this->getMesh(uid);
}

void DataManagerImpl::saveData(DataPtr data, const QString& basePath)
{
    if (!data)
        return;

    ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
    if (image)
    {
        this->saveImage(image, basePath);
        return;
    }

    MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(data);
    if (mesh)
    {
        this->saveMesh(mesh, basePath);
        return;
    }

    // no other implementations..
    messageManager()->sendWarning(QString("Could not save %1 - not implemented").arg(data->getName()));
}


void DataManagerImpl::saveMesh(MeshPtr mesh, const QString& basePath)
{
	vtkPolyDataWriterPtr writer = vtkPolyDataWriterPtr::New();
	writer->SetInput(mesh->getVtkPolyData());
	QString filename = basePath + "/Images/" + mesh->getUid() + ".vtk";
	mesh->setFilename(QDir(basePath).relativeFilePath(filename));
	writer->SetFileName(cstring_cast(filename));

	writer->Update();
	writer->Write();
}

DataPtr DataManagerImpl::getData(const QString& uid) const
{
	DataMap::const_iterator iter = mData.find(uid);
	if (iter == mData.end())
		return DataPtr();
	return iter->second;
}

void DataManagerImpl::verifyParentFrame(DataPtr data)
{
	if (data->getParentSpace().isEmpty())
	{
		int max = 0;
		std::map<QString, DataPtr>::iterator iter;
		for (iter = mData.begin(); iter != mData.end(); ++iter)
		{
			//max = std::max(max, qstring_cast(iter->first).toInt());
			QStringList parentList = qstring_cast(iter->second->getParentSpace()).split("_");
			if (parentList.size() < 2)
				continue;
			max = std::max(max, parentList[1].toInt());
		}
		QString parentFrame = "frame_" + qstring_cast(max + 1);
		data->get_rMd_History()->setParentSpace(parentFrame);
	}
}

ImagePtr DataManagerImpl::getImage(const QString& uid) const
{
	return boost::dynamic_pointer_cast<Image>(this->getData(uid));
}

std::map<QString, DataPtr> DataManagerImpl::getData() const
{
	return mData;
}

std::map<QString, ImagePtr> DataManagerImpl::getImages() const
{
	std::map<QString, ImagePtr> retval;
	for (DataMap::const_iterator iter = mData.begin(); iter != mData.end(); ++iter)
	{
		ImagePtr image = this->getImage(iter->first);
		if (!image)
			continue;
		retval[iter->first] = image;
	}
	return retval;
}

MeshPtr DataManagerImpl::getMesh(const QString& uid) const
{
	return boost::dynamic_pointer_cast<Mesh>(this->getData(uid));
}

std::map<QString, MeshPtr> DataManagerImpl::getMeshes() const
{
	std::map<QString, MeshPtr> retval;
	for (DataMap::const_iterator iter = mData.begin(); iter != mData.end(); ++iter)
	{
		MeshPtr mesh = this->getMesh(iter->first);
		if (!mesh)
			continue;
		retval[iter->first] = mesh;
	}
	return retval;
}

void DataManagerImpl::addXml(QDomNode& parentNode)
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement dataManagerNode = doc.createElement("datamanager");
	parentNode.appendChild(dataManagerNode);

	m_rMpr_History->addXml(dataManagerNode);

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

	QDomElement landmarksNode = doc.createElement("landmarks");
	mPatientLandmarks->addXml(landmarksNode);
	dataManagerNode.appendChild(landmarksNode);

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
//	QDomNode toolManagerNode = dataManagerNode.parentNode().namedItem("toolManager");
//	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");
//	QDomNode toolmanagerNode = managerNode.namedItem("toolManager");
//	toolManager()->parseXml(toolmanagerNode);

	// look in the toolmanager, for backwards compatibility (2014-02-21)
	QDomNode toolManagerNode = dataManagerNode.parentNode().namedItem("toolManager");

	QDomNode registrationHistory = dataManagerNode.namedItem("registrationHistory");
	if (registrationHistory.isNull())
		registrationHistory = toolManagerNode.namedItem("registrationHistory");
	m_rMpr_History->parseXml(registrationHistory);

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

	QDomNode patientLandmarksNode = dataManagerNode.namedItem("landmarks");
	if (patientLandmarksNode.isNull())
		patientLandmarksNode = toolManagerNode.namedItem("landmarks");
	mPatientLandmarks->parseXml(patientLandmarksNode);

	// All images must be created from the DataManager, so the image nodes are parsed here
	std::map<DataPtr, QDomNode> datanodes;

	QDomNode child = dataManagerNode.firstChild();
	for (; !child.isNull(); child = child.nextSibling())
	{
		if (child.nodeName() == "data")
		{
			DataPtr data = this->loadData(child.toElement(), rootPath);
			if (data)
				datanodes[data] = child.toElement();
		}
	}

	// parse xml data separately: we want to first load all data
	// because there might be interdependencies (cx::DistanceMetric)
	for (std::map<DataPtr, QDomNode>::iterator iter = datanodes.begin(); iter != datanodes.end(); ++iter)
	{
		iter->first->parseXml(iter->second);
	}

	emit dataAddedOrRemoved();

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

DataPtr DataManagerImpl::loadData(QDomElement node, QString rootPath)
{
	//  QString uidNodeString = node.namedItem("uid").toElement().text();
	//  QDomElement nameNode = node.namedItem("name").toElement();
	QDomElement filePathNode = node.namedItem("filePath").toElement();

	QString uid = node.toElement().attribute("uid");
	QString name = node.toElement().attribute("name");
	QString type = node.toElement().attribute("type");

	// backwards compatibility 20110306CA
	if (!node.namedItem("uid").toElement().isNull())
		uid = node.namedItem("uid").toElement().text();
	if (!node.namedItem("name").toElement().isNull())
		name = node.namedItem("name").toElement().text();

	if (filePathNode.isNull())
	{
		messageManager()->sendWarning("Warning: DataManager::parseXml() found no filePath for data");
		return DataPtr();
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
		return DataPtr();
	}

//	DataPtr data = this->readData(uid, path, type);

	if (mData.count(uid)) // dont load same image twice
		return mData[uid];

	DataPtr data = mDataFactory->create(type, uid, name);
	if (!data)
	{
		messageManager()->sendWarning(QString("Unknown type: %1 for file %2").arg(type).arg(path));
		return DataPtr();
	}
	bool loaded = data->load(path);

	if (!data || !loaded)
	{
		messageManager()->sendWarning("Unknown file: " + path);
		return DataPtr();
	}

	if (!name.isEmpty())
		data->setName(name);
	data->setFilename(relativePath.path());

	this->loadData(data);

	// conversion for change in format 2013-10-29
	QString newPath = rootPath+"/"+data->getFilename();
	if (QDir::cleanPath(path) != QDir::cleanPath(newPath))
	{
		messageManager()->sendWarning(QString("Detected old data format, converting from %1 to %2").arg(path).arg(newPath));
		this->saveData(data, rootPath);
	}

	return data;
}

void DataManagerImpl::vtkImageDataChangedSlot()
{
	QString uid = "";
	if (mActiveImage)
		uid = mActiveImage->getUid();
}

CLINICAL_APPLICATION DataManagerImpl::getClinicalApplication() const
{
	return mClinicalApplication;
}

void DataManagerImpl::setClinicalApplication(CLINICAL_APPLICATION application)
{
	if (mClinicalApplication == application)
		return;
	mClinicalApplication = application;
	emit clinicalApplicationChanged();
}

int DataManagerImpl::findUniqueUidNumber(QString uidBase) const
{
	if (!uidBase.contains("%"))
		return -1;
	// Find an uid that is not used before
	size_t numMatches = 1;
	int recNumber = 0;

	if (numMatches != 0)
	{
		while (numMatches != 0)
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
ImagePtr DataManagerImpl::createImage(vtkImageDataPtr data, QString uid, QString name, QString filePath)
{
	this->generateUidAndName(&uid, &name);

	ImagePtr retval = ImagePtr(new Image(uid, data, name));
	retval->setAcquisitionTime(QDateTime::currentDateTime());
	retval->setFilename(filePath);

	return retval;
}

/**
 * Create a new image that inherits parameters from a parent
 */
ImagePtr DataManagerImpl::createDerivedImage(vtkImageDataPtr data, QString uid, QString name, ImagePtr parentImage, QString filePath)
{
	ImagePtr retval = this->createImage(data, uid, name, filePath);
	retval->get_rMd_History()->setRegistration(parentImage->get_rMd());
	retval->get_rMd_History()->setParentSpace(parentImage->getUid());
	ImageTF3DPtr transferFunctions = parentImage->getTransferFunctions3D()->createCopy();
	ImageLUT2DPtr LUT2D = parentImage->getLookupTable2D()->createCopy();
	retval->setLookupTable2D(LUT2D);
	retval->setTransferFunctions3D(transferFunctions);
	retval->setModality(parentImage->getModality());
	retval->setImageType(parentImage->getImageType());
	retval->setShading(parentImage->getShading());

	retval->setAcquisitionTime(QDateTime::currentDateTime());

	return retval;
}

/**Insert uid and name containing %1 placeholders for insertion of unique integers.
 * Return unique values.
 * If input does not contain %1, nothing happens.
 */
void DataManagerImpl::generateUidAndName(QString* _uid, QString* _name)
{
	QString& uid = *_uid;
	QString& name = *_name;

	int recNumber = this->findUniqueUidNumber(uid);

	if (uid.contains("%"))
	{
		uid = uid.arg(recNumber);
	}

	if (name.contains("%"))
	{
		if (recNumber == 1)
			name = name.arg("");
		else
			name = name.arg(recNumber);
	}
}

/** Create an image with unique uid. The input uidBase may contain %1 as a placeholder for a running integer that
 *  data manager can increment in order to obtain an unique uid. The same integer will be inserted into nameBase
 *  if %1 is found there
 *
 */
MeshPtr DataManagerImpl::createMesh(vtkPolyDataPtr data, QString uid, QString name, QString filePath)
{
	this->generateUidAndName(&uid, &name);

	MeshPtr retval = MeshPtr(new Mesh(uid, name, data));
	retval->setAcquisitionTime(QDateTime::currentDateTime());
	retval->setFilename(filePath);

	return retval;
}

void DataManagerImpl::removeData(const QString& uid, QString basePath)
{
	if (this->getActiveImage() && this->getActiveImage()->getUid() == uid)
		this->setActiveImage(ImagePtr());

	DataPtr data = this->getData(uid);

	mData.erase(uid);

	this->deleteFiles(data, basePath);

	emit dataAddedOrRemoved(); // this should alert everybody interested in the data as a collection.
	messageManager()->sendInfo("Removed data [" + uid + "].");
}

void DataManagerImpl::deleteFiles(DataPtr data, QString basePath)
{
	if (!data)
		return;
	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	QStringList files;
	if (!data->getFilename().isEmpty())
		files << QDir(basePath).absoluteFilePath(data->getFilename());

	if (image)
	{
		files <<  changeExtension(files[0], "raw");
	}

	for (int i=0; i<files.size(); ++i)
	{
		if (!QFileInfo(files[i]).exists())
			continue;
		messageManager()->sendInfo(QString("Removing %1 from disk").arg(files[i]));
		QFile(files[i]).remove();
	}
}

Transform3D DataManagerImpl::get_rMpr() const
{
	return m_rMpr_History->getCurrentRegistration().mValue;
}

void DataManagerImpl::set_rMpr(const Transform3D& val)
{
	m_rMpr_History->setRegistration(val);
}

RegistrationHistoryPtr DataManagerImpl::get_rMpr_History()
{
	return m_rMpr_History;
}




} // namespace cx


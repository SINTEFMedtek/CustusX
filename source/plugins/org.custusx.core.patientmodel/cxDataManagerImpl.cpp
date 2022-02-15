/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDataManagerImpl.h"

#include <QtCore>
#include <QDomDocument>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "cxTransform3D.h"
#include "cxRegistrationTransform.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxUtilHelpers.h"
#include "cxVideoSource.h"
#include "cxDataLocations.h"

#include "cxImageLUT2D.h"
#include "cxImageTF3D.h"

#include "cxSpaceProvider.h"
#include "cxDataFactory.h"

#include "cxXmlOptionItem.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxProfile.h"
#include "cxSettings.h"
#include "cxActiveData.h"
#include "cxFileManagerService.h"
#include "cxEnumConversion.h"


namespace cx
{

DataManagerImplPtr DataManagerImpl::create(ActiveDataPtr activeData)
{
	DataManagerImplPtr retval;
	retval.reset(new DataManagerImpl(activeData));
	return retval;
}

DataManagerImpl::DataManagerImpl(ActiveDataPtr activeData) :
	mClinicalApplication(mdNEUROLOGICAL),
	mActiveData(activeData)
{
	m_rMpr_History.reset(new RegistrationHistory());
	connect(m_rMpr_History.get(), &RegistrationHistory::currentChanged, this, &DataManager::rMprChanged);
	mPatientLandmarks = Landmarks::create();

	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));
	this->readClinicalView();

	this->clear();
}

DataManagerImpl::~DataManagerImpl()
{
}

void DataManagerImpl::setServices(SpaceProviderPtr spaceProvider, FileManagerServicePtr filemanager)
{
	mSpaceProvider = spaceProvider;
	mFileManagerService = filemanager;
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
	mLandmarkProperties.clear();

	m_rMpr_History->clear();
	mPatientLandmarks->clear();

	emit dataAddedOrRemoved();
	emit centerChanged();
	emit landmarkPropertiesChanged();
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
	if (similar(mCenter, center))
		return;
	mCenter = center;
	emit centerChanged();
}

void DataManagerImpl::setOperatingTable(const OperatingTable &ot)
{
	mOperatingTable = ot;
	emit operatingTableChanged();
}

OperatingTable DataManagerImpl::getOperatingTable() const
{
	return mOperatingTable;
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

void DataManagerImpl::deleteLandmarks()
{
	mLandmarkProperties.clear();
	emit landmarkPropertiesChanged();
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

DataPtr DataManagerImpl::loadData(const QString& uid, const QString& path)
{
	if (mData.count(uid)) // dont load same image twice
		return mData[uid];

	QString type = mFileManagerService->findDataTypeFromFile(path);
	if(!mDataFactory)
		reportError("DataManagerImpl::loadData() Got no DataFactory");
	DataPtr data = mDataFactory->create(type, uid);

	if (!data)
	{
		reportError("Failed to find loaded for: [" + path + "]");
		return DataPtr();
	}

	bool loaded = data->load(path, mFileManagerService);
	//TODO FIX
	//bool loaded = mFileManagerService->load(path, data);

	if (!loaded)
	{
		reportError("Failed to load file: [" + path + "]");
		return DataPtr();
	}

	this->loadData(data);
	return data;
}

void DataManagerImpl::loadData(DataPtr data, bool overWrite)
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
		if (!overWrite && mData.count(data->getUid()) && mData[data->getUid()]!=data)
			reportError(QString("Overwriting Data with uid=%1 with new object into PasM").arg(data->getUid()));
		//this->verifyParentFrame(data);
		mData[data->getUid()] = data;
		emit dataAddedOrRemoved();
	}
}

DataPtr DataManagerImpl::getData(const QString& uid) const
{
	DataMap::const_iterator iter = mData.find(uid);
	if (iter == mData.end())
		return DataPtr();
	return iter->second;
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

	QDomElement otNode = doc.createElement("operatingTable");
	otNode.appendChild(doc.createTextNode(qstring_cast(mOperatingTable.rMot)));
	dataManagerNode.appendChild(otNode);

	for (DataMap::const_iterator iter = mData.begin(); iter != mData.end(); ++iter)
	{
		QDomElement dataNode = doc.createElement("data");
		dataManagerNode.appendChild(dataNode);
		iter->second->addXml(dataNode);
	}
}

void DataManagerImpl::parseXml(QDomNode& dataManagerNode, QString rootPath)
{
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
		if (child.toElement().tagName() == "center")
		{
			const QString centerString = child.toElement().text();
			if (!centerString.isEmpty())
			{
				Vector3D center = Vector3D::fromString(centerString);
				this->setCenter(center);
			}
		}
		if (child.toElement().tagName() == "operatingTable")
		{
			const QString ot = child.toElement().text();
			if (!ot.isEmpty())
			{
				Transform3D tr = Transform3D::fromString(ot);
				OperatingTable t(tr);
				this->setOperatingTable(t);
			}
		}
		child = child.nextSibling();
	}
}

DataPtr DataManagerImpl::loadData(QDomElement node, QString rootPath)
{
	QString uid = node.toElement().attribute("uid");
	QString name = node.toElement().attribute("name");
	QString type = node.toElement().attribute("type");

	QDir relativePath = this->findRelativePath(node, rootPath);
	QString absolutePath = this->findAbsolutePath(relativePath, rootPath);

	if (mData.count(uid)) // dont load same image twice
		return mData[uid];

	DataPtr data = mDataFactory->create(type, uid, name);
	if (!data)
	{
		reportWarning(QString("Unknown type: %1 for file %2").arg(type).arg(absolutePath));
		return DataPtr();
	}
	bool loaded = data->load(absolutePath, mFileManagerService);

	if (!loaded)
	{
		reportWarning("Unknown file: " + absolutePath);
		return DataPtr();
	}

	if (!name.isEmpty())
		data->setName(name);
	data->setFilename(relativePath.path());

	this->loadData(data);

	// conversion for change in format 2013-10-29
	QString newPath = rootPath+"/"+data->getFilename();
	if (QDir::cleanPath(absolutePath) != QDir::cleanPath(newPath))
	{
		reportWarning(QString("Detected old data format, converting from %1 to %2").arg(absolutePath).arg(newPath));
		data->save(rootPath, mFileManagerService);
	}

	return data;
}

QDir DataManagerImpl::findRelativePath(QDomElement node, QString rootPath)
{
	QString path = this->findPath(node);
	QDir relativePath = QDir(QString(path));

	QDir patientDataDir(rootPath);
	relativePath.setPath(patientDataDir.relativeFilePath(relativePath.path()));

	return relativePath;
}

QString DataManagerImpl::findPath(QDomElement node)
{
	QDomElement filePathNode = node.namedItem("filePath").toElement();

	if (filePathNode.isNull())
		return QString();

	QString path = filePathNode.text();
	if (path.isEmpty())
		return QString();
	return path;
}

QString DataManagerImpl::findAbsolutePath(QDir relativePath, QString rootPath)
{
	QString absolutePath = relativePath.path();
	if (!rootPath.isEmpty())
		absolutePath = rootPath + "/" + relativePath.path();
	return absolutePath;
}

CLINICAL_VIEW DataManagerImpl::getClinicalApplication() const
{
	return mClinicalApplication;
}

void DataManagerImpl::setClinicalApplication(CLINICAL_VIEW application)
{
	if (mClinicalApplication == application)
		return;
	mClinicalApplication = application;

	QString val = enum2string<CLINICAL_VIEW>(mClinicalApplication);
	settings()->setValue("View/clinicalView", val);

	emit clinicalApplicationChanged();
}

void DataManagerImpl::settingsChangedSlot(QString key)
{
	if (key == "View/clinicalView")
	{
		this->readClinicalView();
	}
}

void DataManagerImpl::readClinicalView()
{
	QString defVal = enum2string<CLINICAL_VIEW>(mdNEUROLOGICAL);
	QString val = settings()->value("View/clinicalView", defVal).toString();
	CLINICAL_VIEW view = string2enum<CLINICAL_VIEW>(val);

	this->setClinicalApplication(view);
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

void DataManagerImpl::removeData(const QString& uid, QString basePath)
{
	DataPtr dataToBeRemoved = this->getData(uid);
	mActiveData->remove(dataToBeRemoved);

	mData.erase(uid);

	this->deleteFiles(dataToBeRemoved, basePath);

	emit dataAddedOrRemoved(); // this should alert everybody interested in the data as a collection.
	report("Removed data [" + uid + "].");
}

void DataManagerImpl::deleteFiles(DataPtr data, QString basePath)
{
	if (!data)
		return;
	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	QStringList files;
	if (!data->getFilename().isEmpty())
	{
		files << QDir(basePath).absoluteFilePath(data->getFilename());
		if (image)
			files <<  changeExtension(files[0], "raw");
	}

	for (int i=0; i<files.size(); ++i)
	{
		if (!QFileInfo(files[i]).exists())
			continue;
		report(QString("Removing %1 from disk").arg(files[i]));
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

RegistrationHistoryPtr DataManagerImpl::get_rMpr_History() const
{
	return m_rMpr_History;
}

PresetTransferFunctions3DPtr DataManagerImpl::getPresetTransferFunctions3D() const
{
	///< create from filename, create trivial document of type name and root node if no file exists.
	XmlOptionFile preset(DataLocations::findConfigFilePath("presets.xml", "/transferFunctions"));
	XmlOptionFile custom = profile()->getXmlSettings().descend("presetTransferFunctions");

	if (!mPresetTransferFunctions3D)
		mPresetTransferFunctions3D.reset(new TransferFunctions3DPresets(preset, custom));

	return mPresetTransferFunctions3D;
}

} // namespace cx


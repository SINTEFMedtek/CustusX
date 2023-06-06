/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxData.h"

#include <QDomDocument>
#include <QDateTime>
#include <QRegExp>

#include <vtkPlane.h>

#include "cxRegistrationTransform.h"
#include "cxTime.h"
#include "cxLandmark.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxLogger.h"
#include "cxBoundingBox3D.h"
#include "cxEnumConversion.h"

namespace cx
{

Data::Data(const QString& uid, const QString& name) :
	mUid(uid), mFilename(""), mRegistrationStatus(rsNOT_REGISTRATED),mOrganType(otUNKNOWN)//, mParentFrame("")
{
	mTimeInfo.mAcquisitionTime = QDateTime::currentDateTime();
	mTimeInfo.mSoftwareAcquisitionTime = QDateTime();
	mTimeInfo.mOriginalAcquisitionTime = QDateTime();

	if (name == "")
		mName = mUid;
	else
		mName = name;
	m_rMd_History.reset(new RegistrationHistory());
	connect(m_rMd_History.get(), &RegistrationHistory::currentChanged, this, &Data::transformChanged);
	connect(m_rMd_History.get(), &RegistrationHistory::currentChanged, this, &Data::transformChangedSlot);

	mLandmarks = Landmarks::create();
}

Data::~Data()
{
}
void Data::setUid(const QString& uid)
{
	mUid = uid;
	emit propertiesChanged();
}

void Data::setName(const QString& name)
{
	mName = name;
	emit propertiesChanged();
}

QString Data::getUid() const
{
	return mUid;
}

QString Data::getName() const
{
	return mName;
}

QString Data::getFilename() const
{
	return mFilename;
}
void Data::setFilename(QString val)
{
	mFilename = val;
}

/**
 * @return Transform from local data space to (data-)ref space
 */
Transform3D Data::get_rMd() const
{
	return m_rMd_History->getCurrentRegistration().mValue;
}

RegistrationHistoryPtr Data::get_rMd_History()
{
	return m_rMd_History;
}

QString Data::getSpace()
{
	return mUid;
}

QString Data::getParentSpace()
{
	return m_rMd_History->getCurrentParentSpace().mUid;
}

std::vector<Vector3D> Data::getPointCloud() const
{
	DoubleBoundingBox3D bb = this->boundingBox();
	std::vector<Vector3D> retval;

	for (unsigned x=0; x<2; ++x)
		for (unsigned y=0; y<2; ++y)
			for (unsigned z=0; z<2; ++z)
				retval.push_back(bb.corner(x,y,z));

//	Transform3D rMd = this->get_rMd();
//	for (unsigned i=0; i<retval.size(); ++i)
//		retval[i] = rMd.coord(retval[i]);

	return retval;
}

void Data::addXml(QDomNode& dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();

	m_rMd_History->addXml(dataNode);

	dataNode.toElement().setAttribute("uid", mUid);
	dataNode.toElement().setAttribute("name", mName);
	dataNode.toElement().setAttribute("type", this->getType());

	QDomElement filePathNode = doc.createElement("filePath");
	filePathNode.appendChild(doc.createTextNode(this->getFilename()));
	dataNode.appendChild(filePathNode);

	QDomElement acqTimeNode = doc.createElement("acqusitionTime");
	acqTimeNode.appendChild(doc.createTextNode(mTimeInfo.mAcquisitionTime.toString(timestampMilliSecondsFormat())));
	dataNode.appendChild(acqTimeNode);


	QDomElement organTypeNode = doc.createElement("organType");
	organTypeNode.appendChild(doc.createTextNode(enum2string(mOrganType)));
	dataNode.appendChild(organTypeNode);

	if (!mLandmarks->getLandmarks().empty())
	{
		QDomElement landmarksNode = doc.createElement("landmarks");
		mLandmarks->addXml(landmarksNode);
		dataNode.appendChild(landmarksNode);
	}
}

void Data::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;

	mOrganType = string2enum<ORGAN_TYPE>(dataNode.namedItem("organType").toElement().text());
	this->guessOrganType();

	QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
	m_rMd_History->parseXml(registrationHistory);

	if (!dataNode.namedItem("acqusitionTime").toElement().isNull())
		mTimeInfo.mAcquisitionTime = QDateTime::fromString(dataNode.namedItem("acqusitionTime").toElement().text(),
			timestampMilliSecondsFormat());

	if (mLandmarks)
		mLandmarks->parseXml(dataNode.namedItem("landmarks"));
}

void Data::guessOrganType()
{
	if((mOrganType != organtypeCOUNT) && (mOrganType != otUNKNOWN))
	{
//		CX_LOG_DEBUG() << "Data::guessOrganType: Already got organ type: " << mOrganType << "  " << this->mName;
		return;
	}
	QString nameWithouthSpaces = this->mName.simplified();
	nameWithouthSpaces.replace(" ", "");
	mOrganType = string2enum<ORGAN_TYPE>(nameWithouthSpaces);
	if((mOrganType != organtypeCOUNT) && (mOrganType != otUNKNOWN))
	{
//		CX_LOG_DEBUG() << "Data::guessOrganType: Found organ: " << mOrganType << " " << enum2string(mOrganType) << ". From name: " << this->mName;
		return;
	}

	QString uidWithouthSpaces = this->mUid.simplified();
	uidWithouthSpaces.replace(" ", "");
	for(int i = int(otUNKNOWN); i < int(organtypeCOUNT); ++i)
	{
		ORGAN_TYPE organType = ORGAN_TYPE(i);
		QString organTypeString = enum2string(organType);
		if(uidWithouthSpaces.contains(organTypeString))
		{
			mOrganType = organType;
//			CX_LOG_DEBUG() << "Data::guessOrganType: Found organ: " << mOrganType << " " << enum2string(mOrganType) << ". From uid: " << this->mUid;
		}
	}

	if(mOrganType == otAIRWAYS)
	{
		if(uidWithouthSpaces.contains("Airways_tubes_centerline"))
			mOrganType =  otAIRWAYS_CENTERLINES;
		else if (uidWithouthSpaces.contains("Airways_tubes_colored"))
			mOrganType = otAIRWAYS_ENHANCED;
	}
	if((mOrganType == organtypeCOUNT) && (mOrganType == otUNKNOWN))
	{
//		CX_LOG_DEBUG() << "Data::guessOrganType: Still no organ match. Check for route to target";
		if(uidWithouthSpaces.contains("rtt_cl_ext"))
		{
			mOrganType = otROUTE_TO_TARGET_EXTENDED;
//			CX_LOG_DEBUG() << "Data::guessOrganType: Found organ: " << mOrganType << " " << enum2string(mOrganType) << ". From uid: " << this->mUid;
		}
		else if(uidWithouthSpaces.contains("rtt_cl"))
		{
			mOrganType = otROUTE_TO_TARGET;
//			CX_LOG_DEBUG() << "Data::guessOrganType: Found organ: " << mOrganType << " " << enum2string(mOrganType) << ". From uid: " << this->mUid;
		}
		else
		{
			mOrganType = otUNKNOWN;
			CX_LOG_DEBUG() << "Data::guessOrganType: Cannot find organ type from uid or name. (Name: " << this->mName << ", uid: " << this->mUid << ")";
		}
	}
}

/**Get the time the data was created from a data source.
 *
 */
QDateTime Data::getAcquisitionTime() const
{
	if (!mTimeInfo.mAcquisitionTime.isNull())
		return mTimeInfo.mAcquisitionTime;
	// quickie implementation: Assume uid contains time on format timestampSecondsFormat():

	// retrieve timestamp as
	QRegExp tsReg("[0-9]{8}T[0-9]{6}");
	if (tsReg.indexIn(mUid) > 0)
	{
		QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
		return datetime;
	}
	return QDateTime();
}

TimeInfo Data::getAdvancedTimeInfo() const
{
	return mTimeInfo;
}

bool Data::isEqual(DataPtr metric)
{
	return streamXml2String(*this) == streamXml2String(*(metric.get()));
}

void Data::setAcquisitionTime(QDateTime time)
{
	mTimeInfo.mAcquisitionTime = time;
}

void Data::setSoftwareAcquisitionTime(QDateTime time)
{
	mTimeInfo.mSoftwareAcquisitionTime = time;
}

void Data::setOriginalAcquisitionTime(QDateTime time)
{
	mTimeInfo.mOriginalAcquisitionTime = time;
}

LandmarksPtr Data::getLandmarks()
{
	return mLandmarks;
}


CoordinateSystem Data::getCoordinateSystem()
{
	return CoordinateSystem(csDATA, this->getUid());
}

//Moved from Image
// methods for defining and storing clip planes. Data does not use these data, this is up to the mapper
void Data::addPersistentClipPlane(vtkPlanePtr plane)
{
	this->addPlane(plane, mPersistentClipPlanes);
}

void Data::addPlane(vtkPlanePtr plane, std::vector<vtkPlanePtr> &planes)
{
	if (std::count(planes.begin(), planes.end(), plane))
		return;
	planes.push_back(plane);
	emit clipPlanesChanged();
}
std::vector<vtkPlanePtr> Data::getAllClipPlanes()
{
	std::vector<vtkPlanePtr> retval = mPersistentClipPlanes;

	for(int i = 0; i < mInteractiveClipPlanes.size(); ++i)
		retval.push_back(mInteractiveClipPlanes[i]);

	if (mInteractiveClipPlane)
		retval.push_back(mInteractiveClipPlane);

	return retval;
}

void Data::clearPersistentClipPlanes()
{
	mPersistentClipPlanes.clear();
	emit clipPlanesChanged();
}

void Data::setInteractiveClipPlane(vtkPlanePtr plane)
{
	mInteractiveClipPlane = plane;
	emit clipPlanesChanged();
}

void Data::addInteractiveClipPlane(vtkPlanePtr plane)
{
	this->addPlane(plane, mInteractiveClipPlanes);
}

void Data::removeInteractiveClipPlane(vtkPlanePtr plane)
{
	for(int i = 0; i < mInteractiveClipPlanes.size(); ++i)
	{
		if(mInteractiveClipPlanes[i] == plane)
			mInteractiveClipPlanes.erase(mInteractiveClipPlanes.begin() + i);
	}

	emit clipPlanesChanged();
}

ORGAN_TYPE Data::getOrganType() const
{
	return mOrganType;
}

void Data::setOrganType(const ORGAN_TYPE &val)
{
	mOrganType = val;
	emit propertiesChanged();
}

} // namespace cx

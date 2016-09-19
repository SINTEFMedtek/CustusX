/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

namespace cx
{

Data::Data(const QString& uid, const QString& name) :
	mUid(uid), mFilename(""), mRegistrationStatus(rsNOT_REGISTRATED)//, mParentFrame("")
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
	return m_rMd_History->getCurrentParentSpace().mValue;
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

	QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
	m_rMd_History->parseXml(registrationHistory);

	if (!dataNode.namedItem("acqusitionTime").toElement().isNull())
		mTimeInfo.mAcquisitionTime = QDateTime::fromString(dataNode.namedItem("acqusitionTime").toElement().text(),
			timestampMilliSecondsFormat());

	if (mLandmarks)
		mLandmarks->parseXml(dataNode.namedItem("landmarks"));
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
} // namespace cx

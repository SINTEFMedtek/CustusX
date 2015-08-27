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
#include "cxRegistrationTransform.h"
#include "cxTime.h"
#include "cxLandmark.h"
#include "cxCoordinateSystemHelpers.h"

namespace cx
{

Data::Data(const QString& uid, const QString& name) :
	mUid(uid), mFilename(""), mRegistrationStatus(rsNOT_REGISTRATED)//, mParentFrame("")
{
	mAcquisitionTime = QDateTime::currentDateTime();

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
	acqTimeNode.appendChild(doc.createTextNode(mAcquisitionTime.toString(timestampMilliSecondsFormat())));
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
		mAcquisitionTime = QDateTime::fromString(dataNode.namedItem("acqusitionTime").toElement().text(),
			timestampMilliSecondsFormat());

	if (mLandmarks)
		mLandmarks->parseXml(dataNode.namedItem("landmarks"));
}

/**Get the time the data was created from a data source.
 *
 */
QDateTime Data::getAcquisitionTime() const
{
	if (!mAcquisitionTime.isNull())
		return mAcquisitionTime;
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

void Data::setAcquisitionTime(QDateTime time)
{
	mAcquisitionTime = time;
}

LandmarksPtr Data::getLandmarks()
{
	return mLandmarks;
}


CoordinateSystem Data::getCoordinateSystem()
{
	return CoordinateSystem(csDATA, this->getUid());
}

} // namespace cx

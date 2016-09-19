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


#ifndef CXREGISTRATIONTRANSFORM_H_
#define CXREGISTRATIONTRANSFORM_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>

#include "cxTransform3D.h"

class QDomNode;

namespace cx
{

/** \brief A registration event and its transform.
 *
 * Encapsulation of a transform and the registration event,
 * i.e the time and kind of registration.
 *
 * \ingroup cx_resource_core_data
 */
class cxResource_EXPORT RegistrationTransform
{
public:
	Transform3D mValue;///< value of transform
	QDateTime mTimestamp; ///< time the transform was registrated.
	QString mType; ///< description of the kind if registration (manual, patient, landmark, coregistration etc)

	QString mFixed;
	QString mMoving;
	bool mTemp;

	RegistrationTransform();
	explicit RegistrationTransform(const Transform3D& value, const QDateTime& timestamp = QDateTime(),
		const QString& type = "", bool tempTransform = false);
	void addXml(QDomNode& parentNode) const; ///< write internal state to node
	void parseXml(QDomNode& dataNode);///< read internal state from node
};

cxResource_EXPORT bool operator<(const RegistrationTransform& lhs, const RegistrationTransform& rhs);
cxResource_EXPORT bool operator==(const RegistrationTransform& lhs, const RegistrationTransform& rhs);

/**\brief Definition of a parent space event.
 *
 * As RegistrationTransform, but the event is a change in the parent space
 * of the Data, as opposed to a change in transform.
 *
 * \sa RegistrationTransform
 *
 * \ingroup sscData
 */
class cxResource_EXPORT ParentSpace
{
public:
	QString mUid;///< parent frame uid
	QDateTime mTimestamp; ///< time the transform was registrated.
	QString mType; ///< description of the kind if registration (manual, patient, landmark, coregistration etc)

	ParentSpace();
	explicit
		ParentSpace(const QString& parentFrame, const QDateTime& timestamp = QDateTime(), const QString& type = "");
	void addXml(QDomNode& parentNode) const; ///< write internal state to node
	void parseXml(QDomNode& dataNode);///< read internal state from node
};

cxResource_EXPORT bool operator<(const ParentSpace& lhs, const ParentSpace& rhs);
cxResource_EXPORT bool operator==(const ParentSpace& lhs, const ParentSpace& rhs);

typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;

/**\brief A history of registration events.
 *
 * A RegistrationHistory describes the registration history of one
 * transform. Normally only the newest transform is used, but it is
 * possible to roll back in time when needed.
 *
 * \ingroup sscData
 */
class cxResource_EXPORT RegistrationHistory: public QObject
{
Q_OBJECT
public:
	virtual void addXml(QDomNode& parentNode) const; ///< write internal state to node
	virtual void parseXml(QDomNode& dataNode);///< read internal state from node

	virtual void setRegistration(const Transform3D& transform);
	virtual void addOrUpdateRegistration(const QDateTime& oldTime, const RegistrationTransform& newTransform);

	virtual void setParentSpace(const QString& newParent);
	virtual void addParentSpace(const QString& newParent);
	virtual void updateParentSpace(const QDateTime& oldTime, const ParentSpace& newParent);

	virtual std::vector<RegistrationTransform> getData() const;
	virtual std::vector<ParentSpace> getParentSpaces() const;
	virtual void removeNewerThan(const QDateTime& timestamp);
	virtual void setActiveTime(const QDateTime& timestamp);
	virtual QDateTime getActiveTime() const;
	virtual RegistrationTransform getCurrentRegistration() const;
	virtual ParentSpace getCurrentParentSpace();
	virtual void clear(); ///< reset all data loaded from xml

	virtual bool isNull() const
	{
		return false;
	}
	static RegistrationHistoryPtr getNullObject();

signals:
	void currentChanged();
private:
	virtual void addParentSpace(const ParentSpace& newParent);
	virtual void addRegistrationInternal(const RegistrationTransform& transform);
	void setCache(const RegistrationTransform& val, const ParentSpace& parent, const QDateTime& timestamp);
	static RegistrationHistoryPtr mNull;
	std::vector<RegistrationTransform> mData; ///< time-sorted list of all registration events.
	std::vector<ParentSpace> mParentSpaces; ///< time-sorted list of all registration events.
	QDateTime mCurrentTime; ///< disregard registrations later than this time. Invalid means use running time.
	RegistrationTransform mTransformCache; ///< cache for the currently active transform
	ParentSpace mParentSpaceCache; ///< cache for the currently active parent frame
};

} // end namespace cx

#endif /*CXREGISTRATIONTRANSFORM_H_*/

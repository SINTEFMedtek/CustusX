/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

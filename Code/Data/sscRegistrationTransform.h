#ifndef SSCREGISTRATIONTRANSFORM_H_
#define SSCREGISTRATIONTRANSFORM_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>

#include "sscTransform3D.h"

class QDomNode;

namespace ssc
{

/**Encapsulation of a transform and the registration event,
 * i.e the time and kind of registration.
 */
class RegistrationTransform
{
public:
	Transform3D mValue;///< value of transform
	QDateTime mTimestamp; ///< time the transform was registrated.
	QString mType; ///< description of the kind if registration (manual, patient, landmark, coregistration etc)

	QString mFixed;
	QString mMoving;

	RegistrationTransform();
	explicit RegistrationTransform(const Transform3D& value, const QDateTime& timestamp = QDateTime(),
		const QString& type = "");
	void addXml(QDomNode& parentNode) const; ///< write internal state to node
	void parseXml(QDomNode& dataNode);///< read internal state from node
};

bool operator<(const RegistrationTransform& lhs, const RegistrationTransform& rhs);
bool operator==(const RegistrationTransform& lhs, const RegistrationTransform& rhs);

/**Encapsulation of a transform and the registration event,
 * i.e the time and kind of registration.
 */
class ParentSpace
{
public:
	QString mValue;///< parent frame uid
	QDateTime mTimestamp; ///< time the transform was registrated.
	QString mType; ///< description of the kind if registration (manual, patient, landmark, coregistration etc)

	ParentSpace();
	explicit
		ParentSpace(const QString& parentFrame, const QDateTime& timestamp = QDateTime(), const QString& type = "");
	void addXml(QDomNode& parentNode) const; ///< write internal state to node
	void parseXml(QDomNode& dataNode);///< read internal state from node
};

bool operator<(const ParentSpace& lhs, const ParentSpace& rhs);
bool operator==(const ParentSpace& lhs, const ParentSpace& rhs);

typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;

/**A RegistrationHistory describes the registration history of one
 * transform. Normally only the newest transform is used, but it is
 * possible to roll back in time when needed.
 */
class RegistrationHistory: public QObject
{
Q_OBJECT
public:
	virtual void addXml(QDomNode& parentNode) const; ///< write internal state to node
	virtual void parseXml(QDomNode& dataNode);///< read internal state from node

	virtual void addRegistration(const RegistrationTransform& transform);
	virtual void setRegistration(const Transform3D& transform);
	virtual void updateRegistration(const QDateTime& oldTime, const RegistrationTransform& newTransform);

	virtual void setParentSpace(const QString& newParent);
	virtual void addParentSpace(const QString& newParent);
	virtual void addParentSpace(const ParentSpace& newParent);
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
	void setCache(const RegistrationTransform& val, const ParentSpace& parent, const QDateTime& timestamp);
	static RegistrationHistoryPtr mNull;
	std::vector<RegistrationTransform> mData; ///< time-sorted list of all registration events.
	std::vector<ParentSpace> mParentSpaces; ///< time-sorted list of all registration events.
	QDateTime mCurrentTime; ///< disregard registrations later than this time. Invalid means use running time.
	RegistrationTransform mTransformCache; ///< cache for the currently active transform
	ParentSpace mParentSpaceCache; ///< cache for the currently active parent frame
};

} // end namespace ssc

#endif /*SSCREGISTRATIONTRANSFORM_H_*/

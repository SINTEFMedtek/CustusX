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

  RegistrationTransform();
  explicit RegistrationTransform(const Transform3D& value, const QDateTime& timestamp=QDateTime(), const QString& type="");
  void addXml(QDomNode& parentNode) const; ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node
};

bool operator<(const RegistrationTransform& lhs, const RegistrationTransform& rhs);
bool operator==(const RegistrationTransform& lhs, const RegistrationTransform& rhs);

/**Encapsulation of a transform and the registration event,
 * i.e the time and kind of registration.
 */
class ParentFrame
{
public:
  QString mValue;///< parent frame uid
  QDateTime mTimestamp; ///< time the transform was registrated.
  QString mType; ///< description of the kind if registration (manual, patient, landmark, coregistration etc)

  ParentFrame();
  explicit ParentFrame(const QString& parentFrame, const QDateTime& timestamp=QDateTime(), const QString& type="");
  void addXml(QDomNode& parentNode) const; ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node
};

bool operator<(const ParentFrame& lhs, const ParentFrame& rhs);
bool operator==(const ParentFrame& lhs, const ParentFrame& rhs);

/**A RegistrationHistory describes the registration history of one
 * transform. Normally only the newest transform is used, but it is
 * possible to roll back in time when needed.
 */
class RegistrationHistory : public QObject
{
  Q_OBJECT
public:
  void addXml(QDomNode& parentNode) const; ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node

  void addRegistration(const RegistrationTransform& transform);
  void setRegistration(const Transform3D& transform);
  void updateRegistration(const QDateTime& oldTime, const RegistrationTransform& newTransform);

  void addParentFrame(const QString& newParent);
  void addParentFrame(const ParentFrame& newParent);
  void updateParentFrame(const QDateTime& oldTime, const ParentFrame& newParent);

  std::vector<RegistrationTransform> getData() const;
  std::vector<ParentFrame> getParentFrames() const;
  void removeNewerThan(const QDateTime& timestamp);
  void setActiveTime(const QDateTime& timestamp);
  QDateTime getActiveTime() const;
  RegistrationTransform getCurrentRegistration() const;
  ParentFrame getCurrentParentFrame();
  void clear(); ///< reset all data loaded from xml
signals:
  void currentChanged();
private:
  void setCache(const RegistrationTransform& val, const ParentFrame& parent, const QDateTime& timestamp);

  std::vector<RegistrationTransform> mData; ///< time-sorted list of all registration events.
  std::vector<ParentFrame> mParentFrames; ///< time-sorted list of all registration events.
  QDateTime mCurrentTime; ///< disregard registrations later than this time. Invalid means use running time.
  RegistrationTransform mTransformCache; ///< cache for the currently active transform
  ParentFrame mParentFrameCache; ///< cache for the currently active parent frame
};
typedef boost::shared_ptr<RegistrationHistory> RegistrationHistoryPtr;

} // end namespace ssc

#endif /*SSCREGISTRATIONTRANSFORM_H_*/

#ifndef SSCREGISTRATIONTRANSFORM_H_
#define SSCREGISTRATIONTRANSFORM_H_

#include <string>
#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "vtkSmartPointer.h"
#include "sscTransform3D.h"

class QDomNode;

namespace ssc
{
//namespace time {

QString timestampSecondsFormat();
QString timestampSecondsFormatNice();

//} // time
} // ssc


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
  bool operator<(const RegistrationTransform& rhs) const;
  bool operator==(const RegistrationTransform& rhs) const;
  void addXml(QDomNode& parentNode); ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node
};

/**A RegistrationHistory describes the registration history of one
 * transform. Normally only the newest transform is used, but it is
 * possible to roll back in time when needed.
 */
class RegistrationHistory : public QObject
{
  Q_OBJECT
public:
  void addXml(QDomNode& parentNode); ///< write internal state to node
  void parseXml(QDomNode& dataNode);///< read internal state from node
  void addRegistration(const RegistrationTransform& transform);
  void setRegistration(const Transform3D& transform);
  std::vector<RegistrationTransform> getData() const;
  void removeNewerThan(const QDateTime& timestamp);
  void setActiveTime(const QDateTime& timestamp);
  QDateTime getActiveTime() const;
  Transform3D getCurrentRegistration() const;
signals:
  void currentChanged();
private:
  void setCache(const Transform3D& val, const QDateTime& timestamp);

  std::vector<RegistrationTransform> mData; ///< time-sorted list of all registration events.
  QDateTime mCurrentTime; ///< disregard registrations later than this time. Invalid means use running time.
  Transform3D mTransformCache; ///< cache for the currently active transform
};
typedef boost::shared_ptr<RegistrationHistory> RegistrationHistoryPtr;

} // end namespace ssc

#endif /*SSCREGISTRATIONTRANSFORM_H_*/

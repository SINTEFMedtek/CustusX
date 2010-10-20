#ifndef SSCLANDMARK_H_
#define SSCLANDMARK_H_

#include <string>
#include <map>
#include <QObject>
#include <QDateTime>
#include "sscVector3D.h"

class QDomNode;

/*
 * sscLandmark.h
 *
 *  Created on: Apr 19, 2010
 *      Author: jbake
 */
namespace ssc
{
class Landmark
{
public:
	explicit Landmark(QString uid="", Vector3D coord=Vector3D(0,0,0));
	~Landmark();

	QString getUid() const;
	ssc::Vector3D getCoord() const;
	QDateTime getTimestamp() const;

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);

private:
	QString mUid;
	ssc::Vector3D mCoord; ///< the landmarks coordinate
	QDateTime mTimestamp; ///< the time at which the landmark was sample
};
typedef std::map<QString, Landmark> LandmarkMap;

class LandmarkProperty
{
public:
  explicit LandmarkProperty(const QString& uid="", const QString& name="", bool active=true);
  bool getActive() const;
  QString getName() const;
  QString getUid() const;
  void setName(const QString& name);
  void setActive(bool active);

  void addXml(QDomNode& dataNode);
  void parseXml(QDomNode& dataNode);

private:
  QString mUid;
  QString mName;
  bool mActive;
};
typedef std::map<QString, LandmarkProperty> LandmarkPropertyMap;


} //namespace ssc

#endif /* SSCLANDMARK_H_ */

#ifndef SSCLANDMARK_H_
#define SSCLANDMARK_H_

#include <QObject>
#include <QDomNode>
#include <QDateTime>
#include <string>
#include <map>
#include "sscVector3D.h"
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
	explicit Landmark(std::string uid="", Vector3D coord=Vector3D(0,0,0));
	~Landmark();

	std::string getUid() const;
	ssc::Vector3D getCoord() const;
	QDateTime getTimestamp() const;

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);

private:
	std::string mUid;
	ssc::Vector3D mCoord; ///< the landmarks coordinate
	QDateTime mTimestamp; ///< the time at which the landmark was sample
};
typedef std::map<std::string, Landmark> LandmarkMap;

class LandmarkProperty
{
public:
  explicit LandmarkProperty(const std::string& uid="", const std::string& name="", bool active=true);
  bool getActive() const;
  std::string getName() const;
  std::string getUid() const;
  void setName(const std::string& name);
  void setActive(bool active);

  void addXml(QDomNode& dataNode);
  void parseXml(QDomNode& dataNode);

private:
  std::string mUid;
  std::string mName;
  bool mActive;
};
typedef std::map<std::string, LandmarkProperty> LandmarkPropertyMap;


} //namespace ssc

#endif /* SSCLANDMARK_H_ */

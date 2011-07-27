/*
 * cxPlaneMetric.h
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#ifndef CXPLANEMETRIC_H_
#define CXPLANEMETRIC_H_


#include "sscData.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscDataManagerImpl.h"

namespace cx
{
typedef boost::shared_ptr<class PlaneMetric> PlaneMetricPtr;

class PlaneMetricReader: public ssc::DataReader
{
public:
  virtual ~PlaneMetricReader() {}
  virtual bool canLoad(const QString& type, const QString& filename)
  {
    return type=="planeMetric";
  }
  virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**Data class that represents an infinite plane defined by all pts r in
 * N*r+c=0, N is the normal, c is a scalar. The plane is defined by setting
 * one of the points r instead of the scalar c.
 *
 * The point is attached to a specific coordinate system / frame.
 */
class PlaneMetric : public ssc::Data
{
  Q_OBJECT
public:
  PlaneMetric(const QString& uid, const QString& name="");
  virtual ~PlaneMetric();

  void setCoordinate(const ssc::Vector3D& p);
  ssc::Vector3D getCoordinate() const;
  void setNormal(const ssc::Vector3D& p);
  ssc::Vector3D getNormal() const;
  void setFrame(ssc::CoordinateSystem space); // use parentframe from ssc::Data
  ssc::CoordinateSystem getFrame() const; // use parentframe from ssc::Data
  virtual QString getType() const { return "planeMetric"; }

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  virtual ssc::DoubleBoundingBox3D boundingBox() const;

private:
  ssc::Vector3D mCoordinate;
  ssc::Vector3D mNormal;
  ssc::CoordinateSystem mFrame;
};

}

#endif /* CXPLANEMETRIC_H_ */

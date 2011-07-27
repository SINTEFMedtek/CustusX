/*
 * cxDistanceMetric.h
 *
 *  Created on: Jul 4, 2011
 *      Author: christiana
 */

#ifndef CXDISTANCEMETRIC_H_
#define CXDISTANCEMETRIC_H_

#include "sscData.h"
#include "cxPointMetric.h"
#include "sscDataManagerImpl.h"

namespace cx
{
typedef boost::shared_ptr<class DistanceMetric> DistanceMetricPtr;


class DistanceMetricReader: public ssc::DataReader
{
public:
  virtual ~DistanceMetricReader() {}
  virtual bool canLoad(const QString& type, const QString& filename)
  {
    return type=="distanceMetric";
  }
  virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**Data class that represents a distance between two points.
 *
 */
class DistanceMetric : public ssc::Data
{
	Q_OBJECT
public:
	DistanceMetric(const QString& uid, const QString& name);
	virtual ~DistanceMetric();

	double getDistance() const;

	void setPoint(int index, PointMetricPtr p);
	PointMetricPtr getPoint(int index);

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  virtual ssc::DoubleBoundingBox3D boundingBox() const;
  virtual QString getType() const { return "distanceMetric"; }

private:
  boost::array<PointMetricPtr,2> mPoint;
};

}

#endif /* CXDISTANCEMETRIC_H_ */

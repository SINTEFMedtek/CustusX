/*
 * cxDistanceMetric.h
 *
 *  Created on: Jul 4, 2011
 *      Author: christiana
 */

#ifndef CXDISTANCEMETRIC_H_
#define CXDISTANCEMETRIC_H_

#include "cxDataMetric.h"
#include "sscDataManagerImpl.h"

namespace cx
{
/**
* \file
* \addtogroup cxServicePatient
* @{
*/

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

/**\brief Data class that represents a distance between two points,
 * or a point and a plane.
 * \ingroup cxServicePatient
 *
 */
class DistanceMetric : public DataMetric
{
	Q_OBJECT
public:
	DistanceMetric(const QString& uid, const QString& name);
	virtual ~DistanceMetric();

	double getDistance() const;
	std::vector<ssc::Vector3D> getEndpoints() const;

	unsigned getArgumentCount() const;
	void setArgument(int index, ssc::DataPtr p);
	ssc::DataPtr getArgument(int index);
	bool validArgument(ssc::DataPtr p) const;

//	void setPoint(int index, PointMetricPtr p);
//	PointMetricPtr getPoint(int index);

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  virtual ssc::DoubleBoundingBox3D boundingBox() const;
  virtual QString getType() const { return "distanceMetric"; }

private:
  boost::array<ssc::DataPtr,2> mArgument;
};

/**
* @}
*/
}

#endif /* CXDISTANCEMETRIC_H_ */

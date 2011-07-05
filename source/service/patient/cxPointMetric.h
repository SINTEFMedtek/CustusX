/*
 * cxPointMetric.h
 *
 *  Created on: Jul 4, 2011
 *      Author: christiana
 */

#ifndef CXPOINTMETRIC_H_
#define CXPOINTMETRIC_H_

#include "sscData.h"
#include "sscCoordinateSystemHelpers.h"

namespace cx
{
typedef boost::shared_ptr<class PointMetric> PointMetricPtr;


/**Data class that represents a single point.
 * The point is attached to a specific coordinate system / frame.
 *
 */
class PointMetric : public ssc::Data
{
	Q_OBJECT
public:
	PointMetric(const QString& uid, const QString& name="");
	virtual ~PointMetric();

	void setCoordinate(const ssc::Vector3D& p);
	ssc::Vector3D getCoordinate() const;
	void setFrame(ssc::CoordinateSystem space); // use parentframe from ssc::Data
	ssc::CoordinateSystem getFrame() const; // use parentframe from ssc::Data

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  virtual ssc::DoubleBoundingBox3D boundingBox() const;

  // additional functionality:
  // - get coord in space
  // - rep

private:
  ssc::Vector3D mCoordinate;
  ssc::CoordinateSystem mFrame;
};

}

#endif /* CXPOINTMETRIC_H_ */

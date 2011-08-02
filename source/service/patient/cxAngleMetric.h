/*
 * cxAngleMetric.h
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#ifndef CXANGLEMETRIC_H_
#define CXANGLEMETRIC_H_

#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "sscDataManagerImpl.h"

namespace cx
{
typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;


class AngleMetricReader: public ssc::DataReader
{
public:
  virtual ~AngleMetricReader() {}
  virtual bool canLoad(const QString& type, const QString& filename)
  {
    return type=="angleMetric";
  }
  virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**Data class that represents an angle between two lines.
 *
 * The lines are defined by four points ABCD. The computed
 * angle is from vector BA to vector CD.
 *
 * The angle ABC can by found by setting the two mid point equal,
 * .ie. ABBC.
 *
 */
class AngleMetric : public DataMetric
{
  Q_OBJECT
public:
  AngleMetric(const QString& uid, const QString& name);
  virtual ~AngleMetric();

  double getAngle() const;
	std::vector<ssc::Vector3D> getEndpoints() const;

	unsigned getArgumentCount() const;
	void setArgument(int index, ssc::DataPtr p);
	ssc::DataPtr getArgument(int index);
	bool validArgument(ssc::DataPtr p) const;
  bool isValid() const;

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  virtual ssc::DoubleBoundingBox3D boundingBox() const;
  virtual QString getType() const { return "angleMetric"; }

private:
  boost::array<ssc::DataPtr,4> mArgument;
};

}

#endif /* CXANGLEMETRIC_H_ */

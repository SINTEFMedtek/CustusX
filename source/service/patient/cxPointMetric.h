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
#include "sscDataManagerImpl.h"

namespace cx
{

/**Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 */
class CoordinateSystemListener : public QObject
{
  Q_OBJECT

public:
  CoordinateSystemListener(ssc::CoordinateSystem space = ssc::CoordinateSystem());
  virtual ~CoordinateSystemListener();
  void setSpace(ssc::CoordinateSystem space);
  ssc::CoordinateSystem getSpace() const;
signals:
  void changed();
private slots:
  void reconnect();
private:
  void doConnect();
  void doDisconnect();
  ssc::CoordinateSystem mSpace;
};
typedef boost::shared_ptr<CoordinateSystemListener> CoordinateSystemListenerPtr;

typedef boost::shared_ptr<class PointMetric> PointMetricPtr;

class PointMetricReader: public ssc::DataReader
{
public:
  virtual ~PointMetricReader() {}
  virtual bool canLoad(const QString& type, const QString& filename)
  {
    return type=="pointMetric";
  }
  virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};


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
	virtual QString getType() const { return "pointMetric"; }
	ssc::Vector3D getRefCoord() const;

  virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
  virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  virtual ssc::DoubleBoundingBox3D boundingBox() const;

  // additional functionality:
  // - get coord in space
  // - rep

private:
  ssc::Vector3D mCoordinate;
  ssc::CoordinateSystem mFrame;
  CoordinateSystemListenerPtr mFrameListener;
};

}

#endif /* CXPOINTMETRIC_H_ */

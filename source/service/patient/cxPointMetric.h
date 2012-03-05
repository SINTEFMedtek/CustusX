// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXPOINTMETRIC_H_
#define CXPOINTMETRIC_H_

#include "cxDataMetric.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscDataManagerImpl.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

/**\brief Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 */
class CoordinateSystemListener: public QObject
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

/** \brief ssc::DataReader implementation for PointMetric
 *
 *  \date Jul 4, 2011
 *  \author christiana
 */
class PointMetricReader: public ssc::DataReader
{
public:
	virtual ~PointMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "pointMetric";
	}
	virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Data class that represents a single point.
 * \ingroup cxServicePatient
 *
 * The point is attached to a specific coordinate system / frame.
 *
 */
class PointMetric: public DataMetric
{
Q_OBJECT
public:
	PointMetric(const QString& uid, const QString& name = "");
	virtual ~PointMetric();

	void setCoordinate(const ssc::Vector3D& p);
	ssc::Vector3D getCoordinate() const;
	void setSpace(ssc::CoordinateSystem space); // use parentframe from ssc::Data
	ssc::CoordinateSystem getSpace() const; // use parentframe from ssc::Data
	virtual QString getType() const
	{
		return "pointMetric";
	}
	ssc::Vector3D getRefCoord() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual ssc::DoubleBoundingBox3D boundingBox() const;

	// additional functionality:
	// - get coord in space
	// - rep

private:
	ssc::Vector3D mCoordinate;
	ssc::CoordinateSystem mSpace;
	CoordinateSystemListenerPtr mSpaceListener;
};

/**
 * @}
 */
}

#endif /* CXPOINTMETRIC_H_ */

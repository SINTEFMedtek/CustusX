// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCPOINTMETRIC_H_
#define SSCPOINTMETRIC_H_

#include "sscDataMetric.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscDataManagerImpl.h"

namespace ssc
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

/**\brief Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
 */
class CoordinateSystemListener: public QObject
{
Q_OBJECT

public:
	CoordinateSystemListener(ssc::CoordinateSystem space);
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
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
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
 *
 * The point is attached to a specific coordinate system / frame.
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
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

#endif /* SSCPOINTMETRIC_H_ */

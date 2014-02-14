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
#include "sscDataReaderWriter.h"
#include "sscCoordinateSystemListener.h"

namespace cx
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class PointMetric> PointMetricPtr;

/** \brief DataReader implementation for PointMetric
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
 */
class PointMetricReader: public DataReader
{
public:
	virtual ~PointMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "pointMetric";
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
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
    static PointMetricPtr create(QDomNode node);
    static PointMetricPtr create(QString uid, QString name="");

	void setCoordinate(const Vector3D& p);
	Vector3D getCoordinate() const;
	void setSpace(CoordinateSystem space); // use parentframe from Data
	CoordinateSystem getSpace() const; // use parentframe from Data
	virtual QString getType() const
	{
		return "pointMetric";
	}
    virtual Vector3D getRefCoord() const;
	virtual QString getAsSingleLineString() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;

	virtual QString getValueAsString() const;
	virtual bool showValueInGraphics() const { return false; }

private:
	Vector3D mCoordinate;
	CoordinateSystem mSpace;
	CoordinateSystemListenerPtr mSpaceListener;
};

/**
 * @}
 */
}

#endif /* SSCPOINTMETRIC_H_ */

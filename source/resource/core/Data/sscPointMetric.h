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
#include "cxOptionalValue.h"

namespace cx
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class PointMetric> PointMetricPtr;

/** \brief Data class that represents a single point.
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
	virtual ~PointMetric();
//    static PointMetricPtr create(QDomNode node);
	static PointMetricPtr create(QString uid, QString name, DataManager* dataManager, SpaceProviderPtr spaceProvider);

	void setCoordinate(const Vector3D& p);
	Vector3D getCoordinate() const;
	void setSpace(CoordinateSystem space); // use parentframe from Data
	CoordinateSystem getSpace() const; // use parentframe from Data
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
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

private slots:
	void resetCachedValues();
private:
	PointMetric(const QString& uid, const QString& name, DataManager* dataManager, SpaceProviderPtr spaceProvider);
	Vector3D mCoordinate;
	CoordinateSystem mSpace;
	SpaceListenerPtr mSpaceListener;
	mutable OptionalValue<Vector3D> mCachedRefCoord;
};

/**
 * @}
 */
}

#endif /* SSCPOINTMETRIC_H_ */

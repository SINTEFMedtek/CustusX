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

#ifndef SSCDISTANCEMETRIC_H_
#define SSCDISTANCEMETRIC_H_

#include "sscDataMetric.h"
#include "cxMetricReferenceArgumentList.h"
#include "cxOptionalValue.h"

namespace cx
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class DistanceMetric> DistanceMetricPtr;

/**\brief Data class that represents a distance between two points,
 * or a point and a plane.
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
 */
class DistanceMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~DistanceMetric();
	static DistanceMetricPtr create(QString uid, QString name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider);

	double getDistance() const;
    std::vector<Vector3D> getEndpoints() const; ///< return the two endpoints in reference space. None if invalid.
    virtual Vector3D getRefCoord() const;
	virtual QString getAsSingleLineString() const;

	MetricReferenceArgumentListPtr getArguments() { return mArguments; }

    virtual bool isValid() const;

    virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "distanceMetric";
	}

	virtual QString getValueAsString() const;
	virtual bool showValueInGraphics() const { return true; }

private slots:
	void resetCachedValues();
private:
	DistanceMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider);
	std::vector<Vector3D> getEndpointsUncached() const;
	MetricReferenceArgumentListPtr mArguments;
	mutable OptionalValue<std::vector<Vector3D> > mCachedEndPoints;

};

/**
 * @}
 */
}

#endif /* SSCDISTANCEMETRIC_H_ */

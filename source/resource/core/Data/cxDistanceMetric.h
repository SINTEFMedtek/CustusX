/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXDISTANCEMETRIC_H_
#define CXDISTANCEMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxMetricReferenceArgumentList.h"
#include "cxOptionalValue.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class DistanceMetric> DistanceMetricPtr;

/**\brief Data class that represents a distance between two points,
 * or a point and a plane.
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT DistanceMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~DistanceMetric();
	static DistanceMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	virtual QIcon getIcon() {return QIcon(":/icons/metric_distance.png");}
	/** The distance from p0 to p1 along the direction
	 */
	double getDistance() const;
	/** Return the direction of the distance vector.
	 * This is usually from (p1-p1).norm(), except when a plane is part of the definition.
	 * In that case the plane normal is used, allowing for signed distances.
	 */
	Vector3D getDirection() const;
	std::vector<Vector3D> getEndpoints() const; ///< return the two endpoints in reference space. None if invalid.
    virtual Vector3D getRefCoord() const;

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
	DistanceMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	void getEndpointsUncached(std::vector<Vector3D>* endpoints, Vector3D *direction) const;
	MetricReferenceArgumentListPtr mArguments;
	mutable OptionalValue<std::vector<Vector3D> > mCachedEndPoints;
	mutable OptionalValue<Vector3D> mCachedDirection;

	void updateCache() const;
};

/**
 * @}
 */
}

#endif /* CXDISTANCEMETRIC_H_ */

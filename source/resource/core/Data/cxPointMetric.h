/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPOINTMETRIC_H_
#define CXPOINTMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxOptionalValue.h"
#include "cxCoordinateSystemHelpers.h"

namespace cx
{
struct CoordinateSystem;

/**
 * \file
 * \addtogroup cx_resource_core_data
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
class cxResource_EXPORT PointMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~PointMetric();
	static PointMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	virtual QString getParentSpace();

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
	virtual QIcon getIcon() {return QIcon(":/icons/metric_point.png");}
	virtual Vector3D getRefCoord() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;

	virtual QString getValueAsString() const;
	virtual bool showValueInGraphics() const { return false; }

private slots:
	void resetCachedValues();
private:
	PointMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	Vector3D mCoordinate;
	CoordinateSystem mSpace;
	SpaceListenerPtr mSpaceListener;
	mutable OptionalValue<Vector3D> mCachedRefCoord;
};

/**
 * @}
 */
}

#endif /* CXPOINTMETRIC_H_ */

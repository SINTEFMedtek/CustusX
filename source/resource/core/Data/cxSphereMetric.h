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
#ifndef CXSPHEREMETRIC_H
#define CXSPHEREMETRIC_H

#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "cxDataManagerImpl.h"
#include "cxMetricReferenceArgumentList.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class SphereMetric> SphereMetricPtr;

/**\brief Data class that represents a donut.
 *
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class SphereMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~SphereMetric();
//	static SphereMetricPtr create(QDomNode node);
	static SphereMetricPtr create(QString uid, QString name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider);

	MetricReferenceArgumentListPtr getArguments() { return mArguments; }
	virtual bool isValid() const;
	virtual QString getAsSingleLineString() const;

	void setRadius(double val);
	double getRadius() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;
	virtual Vector3D getRefCoord() const;
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "SphereMetric";
	}
	virtual QString getValueAsString() const;
	virtual bool showValueInGraphics() const { return false; }

private:
	SphereMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider);
	MetricReferenceArgumentListPtr mArguments;
	double mRadius;
};

/**
 * @}
 */
} // namespace cx


#endif // CXSPHEREMETRIC_H

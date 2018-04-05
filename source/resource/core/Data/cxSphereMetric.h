/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSPHEREMETRIC_H
#define CXSPHEREMETRIC_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "cxMetricReferenceArgumentList.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class SphereMetric> SphereMetricPtr;

/** \brief Data class that represents a donut.
 *
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT SphereMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~SphereMetric();
	static SphereMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	MetricReferenceArgumentListPtr getArguments() { return mArguments; }
	virtual bool isValid() const;

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
	virtual QIcon getIcon() {return QIcon(":/icons/metric_sphere.png");}
	virtual QString getValueAsString() const;
	virtual bool showValueInGraphics() const { return false; }

private:
	SphereMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	MetricReferenceArgumentListPtr mArguments;
	double mRadius;
};

/**
 * @}
 */
} // namespace cx


#endif // CXSPHEREMETRIC_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSHAPEDMETRIC_H
#define CXSHAPEDMETRIC_H

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

typedef boost::shared_ptr<class DonutMetric> DonutMetricPtr;

/** \brief Data class that represents a donut.
 *
 * The donut can be used as a targeting device, for example using needle injection.
 * Use a point metric and a donut metric in a line, and target by visually placing
 * the point inside the donut to aim along the line.
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT DonutMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~DonutMetric();
	static DonutMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	virtual bool isValid() const;
	virtual QIcon getIcon() {return QIcon(":/icons/metric_torus.png");}

	void setRadius(double val);
	double getRadius() const;
	void setThickness(double val);
	double getThickness() const;
	void setHeight(double val);
	double getHeight() const;
	bool getFlat() const;
	void setFlat(bool val);

	Vector3D getPosition();
	Vector3D getDirection();

	MetricReferenceArgumentListPtr getArguments() { return mArguments; }
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
		return "DonutMetric";
	}
	virtual QString getValueAsString() const { return ""; }
	virtual bool showValueInGraphics() const { return false; }

private:
	DonutMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	MetricReferenceArgumentListPtr mArguments;
	double mRadius;
	double mThickness;
	double mHeight;
	bool mFlat;
};

/**
 * @}
 */
} // namespace cx

#endif // CXSHAPEDMETRIC_H

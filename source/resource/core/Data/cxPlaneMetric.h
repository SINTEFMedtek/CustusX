/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPLANEMETRIC_H_
#define CXPLANEMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxPointMetric.h"
#include "cxMetricReferenceArgumentList.h"

namespace cx
{
typedef Eigen::Hyperplane<double, 3> Plane3D;

/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class PlaneMetric> PlaneMetricPtr;

/**
 * \brief Data class representing a plane.
 *
 * Data class that represents an infinite plane defined by all pts r in
 * N*r+c=0, N is the normal, c is a scalar. The plane is defined by setting
 * one of the points r instead of the scalar c.
 *
 * The point is attached to a specific coordinate system / frame.
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT PlaneMetric: public DataMetric
{
Q_OBJECT
public:
	PlaneMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	virtual ~PlaneMetric();
	static PlaneMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

    virtual Vector3D getRefCoord() const;
	Vector3D getRefNormal() const;
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "planeMetric";
	}
	virtual QIcon getIcon() {return QIcon(":/icons/metric_plane.png");}

	Plane3D getRefPlane() const;
	MetricReferenceArgumentListPtr getArguments() { return mArguments; }

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;

	virtual QString getValueAsString() const { return ""; }
	virtual bool showValueInGraphics() const { return false; }

private:
	MetricReferenceArgumentListPtr mArguments;
};

/**
 * @}
 */
}

#endif /* CXPLANEMETRIC_H_ */

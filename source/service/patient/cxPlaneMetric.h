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

#ifndef CXPLANEMETRIC_H_
#define CXPLANEMETRIC_H_

#include "cxDataMetric.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscDataManagerImpl.h"

namespace cx
{
typedef Eigen::Hyperplane<double, 3> Plane3D;

/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

typedef boost::shared_ptr<class PlaneMetric> PlaneMetricPtr;

/**\brief ssc::DataReader implementation for PlaneMetric
 *
 *  \date Jul 27, 2011
 *  \author christiana
 */
class PlaneMetricReader: public ssc::DataReader
{
public:
	virtual ~PlaneMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "planeMetric";
	}
	virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**
 * \brief Data class representing a plane.
 * \ingroup cxServicePatient
 *
 * Data class that represents an infinite plane defined by all pts r in
 * N*r+c=0, N is the normal, c is a scalar. The plane is defined by setting
 * one of the points r instead of the scalar c.
 *
 * The point is attached to a specific coordinate system / frame.
 */
class PlaneMetric: public DataMetric
{
Q_OBJECT
public:
	PlaneMetric(const QString& uid, const QString& name = "");
	virtual ~PlaneMetric();

	void setCoordinate(const ssc::Vector3D& p);
	ssc::Vector3D getCoordinate() const;
	void setNormal(const ssc::Vector3D& p);
	ssc::Vector3D getNormal() const;
	void setSpace(ssc::CoordinateSystem space); // use parentframe from ssc::Data
	ssc::CoordinateSystem getSpace() const; // use parentframe from ssc::Data
	virtual QString getType() const
	{
		return "planeMetric";
	}

	Plane3D getRefPlane() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual ssc::DoubleBoundingBox3D boundingBox() const;

private:
	ssc::Vector3D mCoordinate;
	ssc::Vector3D mNormal;
	ssc::CoordinateSystem mSpace;
};

/**
 * @}
 */
}

#endif /* CXPLANEMETRIC_H_ */

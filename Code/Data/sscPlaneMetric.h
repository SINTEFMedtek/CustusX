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

#ifndef SSCPLANEMETRIC_H_
#define SSCPLANEMETRIC_H_

#include "sscDataMetric.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscDataManagerImpl.h"
#include "sscPointMetric.h"

namespace ssc
{
typedef Eigen::Hyperplane<double, 3> Plane3D;

/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class PlaneMetric> PlaneMetricPtr;

/**\brief ssc::DataReader implementation for PlaneMetric
 *
 *  \date Jul 27, 2011
 *  \author Christian Askeland, SINTEF
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
	CoordinateSystemListenerPtr mSpaceListener;
};

/**
 * @}
 */
}

#endif /* SSCPLANEMETRIC_H_ */

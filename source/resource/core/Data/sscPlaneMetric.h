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

namespace cx
{
typedef Eigen::Hyperplane<double, 3> Plane3D;

/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class PlaneMetric> PlaneMetricPtr;

///**\brief DataReader implementation for PlaneMetric
// *
// *  \date Jul 27, 2011
// *  \author Christian Askeland, SINTEF
// */
//class PlaneMetricReader: public DataReader
//{
//public:
//	virtual ~PlaneMetricReader()
//	{
//	}
//	virtual bool canLoad(const QString& type, const QString& filename)
//	{
//		return type == "planeMetric";
//	}
//	virtual DataPtr load(const QString& uid, const QString& filename);
//};

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
	PlaneMetric(const QString& uid, const QString& name, DataManager* dataManager, SpaceProviderPtr spaceProvider);
	virtual ~PlaneMetric();
//    static PlaneMetricPtr create(QDomNode node);
	static PlaneMetricPtr create(QString uid, QString name, DataManager* dataManager, SpaceProviderPtr spaceProvider);

	void setCoordinate(const Vector3D& p);
	Vector3D getCoordinate() const;
    virtual Vector3D getRefCoord() const;
    void setNormal(const Vector3D& p);
	Vector3D getNormal() const;
	void setSpace(CoordinateSystem space); // use parentframe from Data
	CoordinateSystem getSpace() const; // use parentframe from Data
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "planeMetric";
	}
	virtual QString getAsSingleLineString() const;

	Plane3D getRefPlane() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;

	virtual QString getValueAsString() const { return ""; }
	virtual bool showValueInGraphics() const { return false; }

private:
	Vector3D mCoordinate;
	Vector3D mNormal;
	CoordinateSystem mSpace;
	CoordinateSystemListenerPtr mSpaceListener;
};

/**
 * @}
 */
}

#endif /* SSCPLANEMETRIC_H_ */

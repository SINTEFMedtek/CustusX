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

#ifndef CXDISTANCEMETRIC_H_
#define CXDISTANCEMETRIC_H_

#include "cxDataMetric.h"
#include "sscDataManagerImpl.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

typedef boost::shared_ptr<class DistanceMetric> DistanceMetricPtr;

/**\brief ssc::DataReader implementation for DistanceMetric
 *
 */
class DistanceMetricReader: public ssc::DataReader
{
public:
	virtual ~DistanceMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "distanceMetric";
	}
	virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Data class that represents a distance between two points,
 * or a point and a plane.
 * \ingroup cxServicePatient
 *
 *  \date Jul 4, 2011
 *  \author christiana
 */
class DistanceMetric: public DataMetric
{
Q_OBJECT
public:
	DistanceMetric(const QString& uid, const QString& name);
	virtual ~DistanceMetric();

	double getDistance() const;
	std::vector<ssc::Vector3D> getEndpoints() const;

	unsigned getArgumentCount() const;
	void setArgument(int index, ssc::DataPtr p);
	ssc::DataPtr getArgument(int index);
	bool validArgument(ssc::DataPtr p) const;

//	void setPoint(int index, PointMetricPtr p);
//	PointMetricPtr getPoint(int index);

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual ssc::DoubleBoundingBox3D boundingBox() const;
	virtual QString getType() const
	{
		return "distanceMetric";
	}

private:
	boost::array<ssc::DataPtr, 2> mArgument;
};

/**
 * @}
 */
}

#endif /* CXDISTANCEMETRIC_H_ */

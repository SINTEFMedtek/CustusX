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

#ifndef SSCDISTANCEMETRIC_H_
#define SSCDISTANCEMETRIC_H_

#include "sscDataMetric.h"
#include "sscDataReaderWriter.h"

namespace ssc
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class DistanceMetric> DistanceMetricPtr;

/**\brief ssc::DataReader implementation for DistanceMetric
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
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
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
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

#endif /* SSCDISTANCEMETRIC_H_ */

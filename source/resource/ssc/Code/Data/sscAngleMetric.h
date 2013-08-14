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


#ifndef SSCANGLEMETRIC_H_
#define SSCANGLEMETRIC_H_

#include "sscDataMetric.h"
#include "sscPointMetric.h"
#include "sscDataManagerImpl.h"

namespace ssc
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;

/** \brief ssc::DataReader implementation for AngleMetric
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class AngleMetricReader: public ssc::DataReader
{
public:
	virtual ~AngleMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "angleMetric";
	}
	virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Data class that represents an angle between two lines.
 *
 * The lines are defined by four points ABCD. The computed
 * angle is from vector BA to vector CD.
 *
 * The angle ABC can by found by setting the two mid point equal,
 * .ie. ABBC.
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class AngleMetric: public DataMetric
{
Q_OBJECT
public:
	AngleMetric(const QString& uid, const QString& name);
	virtual ~AngleMetric();

	double getAngle() const;
	std::vector<ssc::Vector3D> getEndpoints() const;

	unsigned getArgumentCount() const;
	void setArgument(int index, ssc::DataPtr p);
	ssc::DataPtr getArgument(int index);
	bool validArgument(ssc::DataPtr p) const;
	bool isValid() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual ssc::DoubleBoundingBox3D boundingBox() const;
	virtual QString getType() const
	{
		return "angleMetric";
	}

private:
	boost::array<DataPtr, 4> mArgument;
};

/**
 * @}
 */
}

#endif /* SSCANGLEMETRIC_H_ */

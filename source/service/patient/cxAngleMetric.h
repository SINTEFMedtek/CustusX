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

#ifndef CXANGLEMETRIC_H_
#define CXANGLEMETRIC_H_

#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "sscDataManagerImpl.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;

/** \brief ssc::DataReader implementation for AngleMetric
 *
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
 * \ingroup cxServicePatient
 *
 * The lines are defined by four points ABCD. The computed
 * angle is from vector BA to vector CD.
 *
 * The angle ABC can by found by setting the two mid point equal,
 * .ie. ABBC.
 *
 * \date Jul 27, 2011
 * \author christiana
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
	boost::array<ssc::DataPtr, 4> mArgument;
};

/**
 * @}
 */
}

#endif /* CXANGLEMETRIC_H_ */

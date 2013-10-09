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

namespace cx
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;

/** \brief DataReader implementation for AngleMetric
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class AngleMetricReader: public DataReader
{
public:
	virtual ~AngleMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "angleMetric";
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
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
    static AngleMetricPtr create(QDomNode node);
    static AngleMetricPtr create(QString uid, QString name="");

	double getAngle() const;
	std::vector<Vector3D> getEndpoints() const;

	unsigned getArgumentCount() const;
	void setArgument(int index, DataPtr p);
	DataPtr getArgument(int index);
	bool validArgument(DataPtr p) const;
    virtual bool isValid() const;
	virtual QString getAsSingleLineString() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;
    virtual Vector3D getRefCoord() const;
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

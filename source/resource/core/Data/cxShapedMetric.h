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
#ifndef CXSHAPEDMETRIC_H
#define CXSHAPEDMETRIC_H

#include "sscDataMetric.h"
#include "sscPointMetric.h"
#include "sscDataManagerImpl.h"
#include "cxMetricReferenceArgumentList.h"

namespace cx
{
/**
 * \file
 * \addtogroup sscData
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
class DonutMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~DonutMetric();
	static DonutMetricPtr create(QString uid, QString name, DataManager* dataManager, SpaceProviderPtr spaceProvider);

	virtual bool isValid() const;
	virtual QString getAsSingleLineString() const;

	void setRadius(double val);
	double getRadius() const;
	void setThickness(double val);
	double getThickness() const;
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
	DonutMetric(const QString& uid, const QString& name, DataManager* dataManager, SpaceProviderPtr spaceProvider);
	MetricReferenceArgumentListPtr mArguments;
	double mRadius;
	double mThickness;
	bool mFlat;
};

/**
 * @}
 */
} // namespace cx

#endif // CXSHAPEDMETRIC_H

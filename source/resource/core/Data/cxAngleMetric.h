/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#ifndef CXANGLEMETRIC_H_
#define CXANGLEMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "cxMetricReferenceArgumentList.h"
#include "cxOptionalValue.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;

/** \brief Data class that represents an angle between two lines.
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
class cxResource_EXPORT AngleMetric : public DataMetric
{
Q_OBJECT
public:
	virtual ~AngleMetric();
	static AngleMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	double getAngle() const;
	std::vector<Vector3D> getEndpoints() const;
	virtual QIcon getIcon() {return QIcon(":/icons/metric_angle.png");}
	MetricReferenceArgumentListPtr getArguments() { return mArguments; }
    virtual bool isValid() const;

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
		return "angleMetric";
	}

	virtual QString getValueAsString() const;
	virtual bool showValueInGraphics() const { return true; }
	bool getUseSimpleVisualization() const;
	void setUseSimpleVisualization(bool val);

private slots:
	void resetCachedValues();
private:
	AngleMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	boost::array<DataPtr, 4> mArgument;
	MetricReferenceArgumentListPtr mArguments;
	bool mUseSimpleVisualization;
	mutable OptionalValue<std::vector<Vector3D> > mCachedEndPoints;

};

/**
 * @}
 */
}

#endif /* CXANGLEMETRIC_H_ */

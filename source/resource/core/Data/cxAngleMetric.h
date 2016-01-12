/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
//    static AngleMetricPtr create(QDomNode node);
	static AngleMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	double getAngle() const;
	std::vector<Vector3D> getEndpoints() const;
	virtual QIcon getIcon() {return QIcon(":/icons/metric_angle.png");}


	MetricReferenceArgumentListPtr getArguments() { return mArguments; }
    virtual bool isValid() const;
	virtual QString getAsSingleLineString() const;

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

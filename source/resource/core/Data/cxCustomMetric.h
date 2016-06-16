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
#ifndef CXCUSTOMMETRIC_H
#define CXCUSTOMMETRIC_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "cxMetricReferenceArgumentList.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class CustomMetric> CustomMetricPtr;

/** \brief Data class that represents a custom.
 *
 * The custom can be used as a targeting device, for example using needle injection.
 * Use a point metric and a custom metric in a line, and target by visually placing
 * the point inside the custom to aim along the line.
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT CustomMetric: public DataMetric
{
Q_OBJECT
public:
    virtual ~CustomMetric();
    static CustomMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	virtual bool isValid() const;
	virtual QString getAsSingleLineString() const;
	virtual QIcon getIcon() {return QIcon(":/icons/metric_torus.png");}

	void setRadius(double val);
	double getRadius() const;
	void setThickness(double val);
	double getThickness() const;
	void setHeight(double val);
	double getHeight() const;
	bool getFlat() const;
	void setFlat(bool val);
    QString getDefineVectorUpMethod() const;
    void setDefineVectorUpMethod(QString defineVectorUpMethod);
    void setSTLFile(QString val);
    QString getSTLFile() const;

    Vector3D getPosition() const;
    Vector3D getDirection() const;
    Vector3D getVectorUp() const;

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
        return "CustomMetric";
	}
	virtual QString getValueAsString() const { return ""; }
	virtual bool showValueInGraphics() const { return false; }

private:
    struct cxResource_EXPORT DefineVectorUpMethods
    {
        DefineVectorUpMethods()
        {
            table = "tableDefinesUp";
            connectedFrameInP1 = "connectedFrameDefinesUp";
        }
        QString table;
        QString connectedFrameInP1;
        QStringList getAvailableDefineVectorUpMethods() const;
        std::map<QString, QString> getAvailableDefineVectorUpMethodsDisplayNames() const;
    };

    CustomMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	MetricReferenceArgumentListPtr mArguments;
	double mRadius;
	double mThickness;
	double mHeight;
	bool mFlat;
    QString mDefineVectorUpMethod;
    QString mSTLFile;
    DefineVectorUpMethods mDefineVectorUpMethods;

public:
    CustomMetric::DefineVectorUpMethods getDefineVectorUpMethods() const;
};

/**
 * @}
 */
} // namespace cx

#endif // CXSHAPEDMETRIC_H

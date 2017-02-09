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



#ifndef CXDATAMETRIC_H_
#define CXDATAMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxData.h"
#include <QColor>
#include <cxCoordinateSystemHelpers.h>

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;

/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

/** \brief Base class for all Data Metrics
 *
 * Data Metrics are Data subclasses that aims to add
 * measurements to the system, such as points, angles and
 * distances.
 *
 * \date Aug 2, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT DataMetric : public Data
{
public:
	DataMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	virtual ~DataMetric();

	virtual QIcon getIcon() {return QIcon(":/icons/metric.png");}

	// OBSOBS, Fjern! antakelig ikke bra.
	//virtual QString getSpace();
	virtual void setSpace(CoordinateSystem cs) {}
	virtual CoordinateSystem getSpace() const { return cx::CoordinateSystem::reference(); } // use parentframe from Data
    virtual Vector3D getRefCoord() const = 0;
	virtual QString getAsSingleLineString() const = 0;
	//virtual void updateFromSingleLineString(QStringList& paramterList) {};
    virtual Transform3D getRefFrame() const { return createTransformTranslate(this->getRefCoord()); }
	virtual bool isValid() const { return true; }
	virtual QString getValueAsString() const = 0;
	virtual bool showValueInGraphics() const { return false; }

	void setColor(const QColor& color);
	QColor getColor();

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);
	virtual bool load(QString path) { return true; } ///< metrics don't store info in file, xml only.
	virtual void save(const QString& basePath) {} ///< metrics don't store info in file, xml only.

protected:
	PatientModelServicePtr mDataManager;
	SpaceProviderPtr mSpaceProvider;

	QString getSingleLineHeader() const;
	QColor mColor;

};
typedef boost::shared_ptr<DataMetric> DataMetricPtr;

/**
 * @}
 */
}

#endif /* CXDATAMETRIC_H_ */

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#ifndef CXDATAMETRIC_H_
#define CXDATAMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxData.h"
#include <QColor>

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

    virtual QString getSpace();
    virtual Vector3D getRefCoord() const = 0;
	virtual Transform3D getRefFrame() const { return createTransformTranslate(this->getRefCoord()); }
	virtual bool isValid() const { return true; }
	virtual QString getValueAsString() const = 0;
	virtual bool showValueInGraphics() const { return false; }

	void setColor(const QColor& color);
	QColor getColor();

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);
	virtual bool load(QString path, FileManagerServicePtr filemanager); ///< metrics don't store info in file, xml only.
	virtual void save(const QString& basePath, FileManagerServicePtr filemanager) {} ///< metrics don't store info in file, xml only.

protected:
	PatientModelServicePtr mDataManager;
	SpaceProviderPtr mSpaceProvider;

	QColor mColor;

};
typedef boost::shared_ptr<DataMetric> DataMetricPtr;

/**
 * @}
 */
}

#endif /* CXDATAMETRIC_H_ */

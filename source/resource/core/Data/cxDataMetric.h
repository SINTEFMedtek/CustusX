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


#ifndef CXDATAMETRIC_H_
#define CXDATAMETRIC_H_

#include "cxData.h"
#include <QColor>

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;
class DataManager;

/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

/**\brief Base class for all Data Metrics
 *
 * Data Metrics are Data subclasses that aims to add
 * measurements to the system, such as points, angles and
 * distances.
 *
 * \date Aug 2, 2011
 * \author Christian Askeland, SINTEF
 */
class DataMetric: public Data
{
public:
	DataMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider);
	virtual ~DataMetric();

    virtual QString getSpace();
    virtual Vector3D getRefCoord() const = 0;
	virtual QString getAsSingleLineString() const = 0;
	virtual bool isValid() const { return true; }
	virtual QString getValueAsString() const = 0;
	virtual bool showValueInGraphics() const { return false; }

	void setColor(const QColor& color);
	QColor getColor();

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);
	virtual bool load(QString path) { return true; } ///< metrics dont store info in file, xml only.

protected:
	DataServicePtr mDataManager;
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

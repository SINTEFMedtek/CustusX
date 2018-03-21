/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGIONOFINTERESTMETRIC_H
#define CXREGIONOFINTERESTMETRIC_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxMetricReferenceArgumentList.h"
#include "cxOptionalValue.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

/**
 * Representation of a Region of Interest,
 * a way to define an extent in 3D.
 *
 * The indended usage is as a bounding box,
 * although the internal representation is given in
 * points. This eases coordinate transformations.
 */
class cxResource_EXPORT RegionOfInterest
{
public:
	RegionOfInterest();
	bool isValid() const { return !mPoints.empty(); }
	/**
	 * Calculate a bounding box based on the points and margin,
	 * oriented in the space q, assuming the points are in space d.
	 */
	DoubleBoundingBox3D getBox(Transform3D qMd = Transform3D::Identity());

	double mMargin;
	std::vector<Vector3D> mPoints;
	std::vector<Vector3D> mMaxBoundsPoints;

private:
	DoubleBoundingBox3D generateROIFromPointsAndMargin(const std::vector<Vector3D> &points, double margin) const;
	std::vector<Vector3D> transform(const std::vector<Vector3D> &points, Transform3D M) const;
};

typedef boost::shared_ptr<class RegionOfInterestMetric> RegionOfInterestMetricPtr;

/**
 * Description of a region of interest, defined by a collection of other Data.
 *
 */
class cxResource_EXPORT RegionOfInterestMetric: public DataMetric
{
Q_OBJECT
public:
	virtual ~RegionOfInterestMetric();
	static RegionOfInterestMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	virtual Vector3D getRefCoord() const;
	virtual QString getAsSingleLineString() const;

	virtual bool isValid() const;

	virtual void addXml(QDomNode& dataNode);
	virtual void parseXml(QDomNode& dataNode);
	virtual DoubleBoundingBox3D boundingBox() const;
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "roiMetric";
	}

	virtual QString getValueAsString() const;
	virtual bool showValueInGraphics() const { return false; }

	QStringList getDataList() { return mContainedData; }
	void setDataList(QStringList val);

	bool getUseActiveTooltip() { return mUseActiveTooltip; }
	void setUseActiveTooltip(bool val);

	double getMargin() { return mMargin; }
	void setMargin(double val);

	QString getMaxBoundsData() { return mMaxBoundsData; }
	void setMaxBoundsData(QString val);

	RegionOfInterest getROI() const; // return a ROI in ref space.

private:
	RegionOfInterestMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	QString mMaxBoundsData;
	QStringList mContainedData;
	bool mUseActiveTooltip;
	double mMargin;

	std::vector<SpaceListenerPtr> mListeners;
	void listenTo(CoordinateSystem space);
	void onContentTransformsChanged();
	void onContentChanged();
	Vector3D getToolTip_r() const;
};

/**
 * @}
 */
}

#endif // CXREGIONOFINTERESTMETRIC_H

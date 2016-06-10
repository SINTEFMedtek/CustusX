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

//	virtual QIcon getIcon() {return QIcon(":/icons/metric.png");}
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

//	DoubleBoundingBox3D getMaxROI() const;
//	DoubleBoundingBox3D getBasicROI() const;

	QString mMaxBoundsData;
	QStringList mContainedData;
	bool mUseActiveTooltip;
	double mMargin;

	std::vector<SpaceListenerPtr> mListeners;
	std::vector<Vector3D> getCorners_r(DataPtr data) const;
//	DoubleBoundingBox3D generateROIFromPointsAndMargin(const std::vector<Vector3D> &points, double margin) const;
	void listenTo(CoordinateSystem space);
	void onContentTransformsChanged();
	void onContentChanged();
//	std::vector<Vector3D> getCorners_r_FromNonROI(std::map<QString, DataPtr> data) const;
	Vector3D getToolTip_r() const;
};

/**
 * @}
 */
}

#endif // CXREGIONOFINTERESTMETRIC_H

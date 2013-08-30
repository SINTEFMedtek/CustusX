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

#ifndef CXTESTMETRICFIXTURE_H_
#define CXTESTMETRICFIXTURE_H_

#include "cxFrameMetric.h"
#include "cxToolMetric.h"
#include "sscDistanceMetric.h"
#include "sscPointMetric.h"
#include "sscPlaneMetric.h"
//#include "cxToolMetric.h"
#include <QDomNode>

namespace cxtest {

struct PlaneMetricData
{
    typedef ssc::PlaneMetric METRIC_TYPE;
    ssc::PlaneMetricPtr mMetric;

    ssc::Vector3D mPoint;
    ssc::Vector3D mNormal;
    ssc::CoordinateSystem mSpace;
};

struct PointMetricData
{
    typedef ssc::PointMetric METRIC_TYPE;
    ssc::PointMetricPtr mMetric;

    ssc::Vector3D mPoint;
    ssc::CoordinateSystem mSpace;
};

/** Helper for testing ToolMetric.
  * Contains all data used to construct the metric,
  * along with a (possibly modified) metric.
  */
struct DistanceMetricData
{
    typedef ssc::DistanceMetric METRIC_TYPE;
    ssc::DistanceMetricPtr mMetric;

    double mDistance;
};

struct FrameMetricData
{
    typedef cx::FrameMetric METRIC_TYPE;
	cx::FrameMetricPtr mMetric;

	ssc::Transform3D m_qMt;
    ssc::CoordinateSystem mSpace;

};

/** Helper for testing ToolMetric.
  * Contains all data used to construct the metric,
  * along with a (possibly modified) metric.
  */
struct ToolMetricData
{
	typedef cx::ToolMetric METRIC_TYPE;
	cx::ToolMetricPtr mMetric;

	ssc::Transform3D m_qMt;
	ssc::CoordinateSystem mSpace;
	double mOffset;
	QString mName;
};


/*
 * cxtestMetricFixture.h
 *
 * \date Aug 20, 2013
 * \author Ole Vegard Solberg, SINTEF
 * \author Christian Askeland, SINTEF
 */
class MetricFixture {
public:
    MetricFixture();
    ~MetricFixture();

    FrameMetricData getFrameMetricData();
	ToolMetricData getToolMetricData();
	PointMetricData getPointMetricData(ssc::Vector3D point);
    PlaneMetricData getPlaneMetricData(ssc::Vector3D point, ssc::Vector3D normal);
    DistanceMetricData getDistanceMetricData(double distance, ssc::DataMetricPtr p0, ssc::DataMetricPtr p1);
    DistanceMetricData getDistanceMetricData(double distance);
	QStringList getSingleLineDataList(ssc::DataMetricPtr metric);

    bool metricEqualsData(FrameMetricData data);
    bool metricEqualsData(DistanceMetricData data);
    bool metricEqualsData(PointMetricData data);
    bool metricEqualsData(PlaneMetricData data);
	bool metricEqualsData(ToolMetricData data);

    template<class DATA>
    bool saveLoadXmlGivesEqualTransform(DATA data)
    {
        QDomNode xmlNode = this->createDummyXmlNode();
        data.mMetric->addXml(xmlNode);

        data.mMetric = DATA::METRIC_TYPE::create(xmlNode);

        return this->metricEqualsData(data);
    }

    QDomNode createDummyXmlNode();
    void setPatientRegistration();

//    QStringList splitStringLineIntoTextComponents(QString line);
    bool verifySingleLineHeader(QStringList list, ssc::DataMetricPtr metric);
};

} //namespace cxtest
#endif // CXTESTMETRICFIXTURE_H_

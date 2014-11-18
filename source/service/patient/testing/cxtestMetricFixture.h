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

#ifndef CXTESTMETRICFIXTURE_H_
#define CXTESTMETRICFIXTURE_H_

#include "cxFrameMetric.h"
#include "cxToolMetric.h"
#include "cxDistanceMetric.h"
#include "cxPointMetric.h"
#include "cxPlaneMetric.h"
//#include "cxToolMetric.h"
#include <QDomNode>
#include "cxtestDummyDataManager.h"
#include "cxMessageListener.h"
#include "cxPatientModelService.h"

namespace cxtest {

struct PlaneMetricWithInput
{
	typedef cx::PlaneMetric METRIC_TYPE;
	cx::PlaneMetricPtr mMetric;

	cx::Vector3D mPoint;
	cx::Vector3D mNormal;
	cx::CoordinateSystem mSpace;
};

struct PointMetricWithInput
{
	typedef cx::PointMetric METRIC_TYPE;
	cx::PointMetricPtr mMetric;

	cx::Vector3D mPoint;
	cx::CoordinateSystem mSpace;
};

/** Helper for testing ToolMetric.
  * Contains all data used to construct the metric,
  * along with a (possibly modified) metric.
  */
struct DistanceMetricWithInput
{
	typedef cx::DistanceMetric METRIC_TYPE;
	cx::DistanceMetricPtr mMetric;

    double mDistance;
};

struct FrameMetricWithInput
{
    typedef cx::FrameMetric METRIC_TYPE;
	cx::FrameMetricPtr mMetric;

	cx::Transform3D m_qMt;
	cx::CoordinateSystem mSpace;

};

/** Helper for testing ToolMetric.
  * Contains all data used to construct the metric,
  * along with a (possibly modified) metric.
  */
struct ToolMetricWithInput
{
	typedef cx::ToolMetric METRIC_TYPE;
	cx::ToolMetricPtr mMetric;

	cx::Transform3D m_qMt;
	cx::CoordinateSystem mSpace;
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

	FrameMetricWithInput getFrameMetricWithInput();
	ToolMetricWithInput getToolMetricWithInput();
	PointMetricWithInput getPointMetricWithInput(cx::Vector3D point);
	PlaneMetricWithInput getPlaneMetricWithInput(cx::Vector3D point, cx::Vector3D normal, cx::DataMetricPtr p0, cx::DataMetricPtr p1);
	DistanceMetricWithInput getDistanceMetricWithInput(double distance, cx::DataMetricPtr p0, cx::DataMetricPtr p1);
	DistanceMetricWithInput getDistanceMetricWithInput(double distance);
	QStringList getSingleLineDataList(cx::DataMetricPtr metric);

	bool inputEqualsMetric(FrameMetricWithInput data);
	bool inputEqualsMetric(DistanceMetricWithInput data);
	bool inputEqualsMetric(PointMetricWithInput data);
	bool inputEqualsMetric(PlaneMetricWithInput data);
	bool inputEqualsMetric(ToolMetricWithInput data);

	template<class METRIC_TYPE>
	boost::shared_ptr<METRIC_TYPE> createTestMetric(QString uid="")
	{
		boost::shared_ptr<METRIC_TYPE> retval;
		retval = mServices->mPatientModelService->createSpecificData<METRIC_TYPE>(uid);
		mServices->mPatientModelService->insertData(retval);
		return retval;

		//return METRIC_TYPE::create(uid, "", this->getDataManager(), this->getSpaceProvider());
	}

    template<class DATA>
    bool saveLoadXmlGivesEqualTransform(DATA data)
    {
        QDomNode xmlNode = this->createDummyXmlNode();
        data.mMetric->addXml(xmlNode);

		data.mMetric = this->createTestMetric<typename DATA::METRIC_TYPE>("");
		data.mMetric->parseXml(xmlNode);

		return this->inputEqualsMetric(data);
    }

    QDomNode createDummyXmlNode();
    void setPatientRegistration();

	bool verifySingleLineHeader(QStringList list, cx::DataMetricPtr metric);

private:
	TestServicesPtr mServices;
	cx::MessageListenerPtr mMessageListener;

	cx::SpaceProviderPtr getSpaceProvider();

};

} //namespace cxtest
#endif // CXTESTMETRICFIXTURE_H_

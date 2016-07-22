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

#include "cxRegionOfInterestMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxPlaneMetric.h"
#include "cxPointMetric.h"
#include "cxXMLNodeWrapper.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"
#include "cxSpaceListener.h"
#include "cxLogger.h"

namespace cx
{


RegionOfInterest::RegionOfInterest() : mMargin(0)
{

}

DoubleBoundingBox3D RegionOfInterest::getBox(Transform3D qMd)
{
	DoubleBoundingBox3D bb = this->generateROIFromPointsAndMargin(this->transform(mPoints, qMd), mMargin);

	DoubleBoundingBox3D bb_max = this->generateROIFromPointsAndMargin(this->transform(mMaxBoundsPoints, qMd), mMargin);

	if (bb_max!=DoubleBoundingBox3D::zero())
		bb = intersection(bb, bb_max);

	return bb;
}

std::vector<Vector3D> RegionOfInterest::transform(const std::vector<Vector3D>& points, Transform3D M) const
{
	std::vector<Vector3D> retval;
	for (unsigned i=0; i<points.size(); ++i)
		retval.push_back(M.coord(points[i]));
	return retval;
}

DoubleBoundingBox3D RegionOfInterest::generateROIFromPointsAndMargin(const std::vector<Vector3D>& points, double margin) const
{
	if (points.empty())
		return DoubleBoundingBox3D::zero();

	DoubleBoundingBox3D bb = DoubleBoundingBox3D::fromCloud(points);
	Vector3D vmargin(margin,margin, margin);
	Vector3D bl = bb.bottomLeft() - vmargin;
	Vector3D tr = bb.topRight() + vmargin;
	bb = DoubleBoundingBox3D(bl, tr);

	return bb;
}

//RegionOfInterest RegionOfInterestMetric::getROI() const
//{
//	DoubleBoundingBox3D bb = this->getBasicROI();

//	DoubleBoundingBox3D bb_max = this->getMaxROI();
//	if (bb_max!=DoubleBoundingBox3D::zero())
//		bb = intersection(bb, bb_max);

//	return bb;
//}



//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------



RegionOfInterestMetric::RegionOfInterestMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mUseActiveTooltip = false;
	mMargin = 20;
}

RegionOfInterestMetricPtr RegionOfInterestMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return RegionOfInterestMetricPtr(new RegionOfInterestMetric(uid, name, dataManager, spaceProvider));
}

RegionOfInterestMetric::~RegionOfInterestMetric()
{
}

Vector3D RegionOfInterestMetric::getRefCoord() const
{
	return this->boundingBox().center();
}

bool RegionOfInterestMetric::isValid() const
{
	return this->getROI().getBox() != DoubleBoundingBox3D::zero();
}

void RegionOfInterestMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	XMLNodeAdder adder(dataNode);
	for (unsigned i=0; i<mContainedData.size(); ++i)
		adder.addTextToElement("content", mContainedData[i]);

	adder.addTextToElement("useActiveTooltip", QString::number(mUseActiveTooltip));
	adder.addTextToElement("margin", QString::number(mMargin));
	adder.addTextToElement("maxBoundsData", mMaxBoundsData);
}

void RegionOfInterestMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	XMLNodeParser parser(dataNode);
	mContainedData = parser.parseTextFromDuplicateElements("content");
	mUseActiveTooltip = parser.parseTextFromElement("useActiveTooltip").toInt();
	mMargin = parser.parseTextFromElement("margin").toDouble();
	mMaxBoundsData = parser.parseTextFromElement("maxBoundsData");

	this->onContentChanged();
}

QString RegionOfInterestMetric::getValueAsString() const
{
	return "bb";
}

void RegionOfInterestMetric::setDataList(QStringList val)
{
	mContainedData = val;
	this->onContentChanged();
}

void RegionOfInterestMetric::setUseActiveTooltip(bool val)
{
	mUseActiveTooltip = val;

	this->onContentChanged();
}

void RegionOfInterestMetric::setMargin(double val)
{
	mMargin = val;
	this->onContentChanged();
}

void RegionOfInterestMetric::setMaxBoundsData(QString val)
{
	mMaxBoundsData = val;
	this->onContentChanged();
}

void RegionOfInterestMetric::onContentChanged()
{
	for (unsigned i=0; i<mListeners.size(); ++i)
	{
		disconnect(mListeners[i].get(), &SpaceListener::changed, this, &RegionOfInterestMetric::onContentTransformsChanged);
	}
	mListeners.clear();

	this->listenTo(CoordinateSystem(csTOOL_OFFSET, "active"));
	for (unsigned i=0; i<mContainedData.size(); ++i)
	{
		this->listenTo(CoordinateSystem(csDATA, mContainedData[i]));
	}
	this->onContentTransformsChanged();
}

void RegionOfInterestMetric::listenTo(CoordinateSystem space)
{
	SpaceListenerPtr listener = mSpaceProvider->createListener();
	listener->setSpace(space);
	connect(listener.get(), &SpaceListener::changed, this, &RegionOfInterestMetric::onContentTransformsChanged);
	mListeners.push_back(listener);
}

void RegionOfInterestMetric::onContentTransformsChanged()
{
	emit transformChanged();
//	CX_LOG_CHANNEL_DEBUG("CA") << "RegionOfInterestMetric::onContentTransformsChanged() ROI=" << this->getROI();
}

DoubleBoundingBox3D RegionOfInterestMetric::boundingBox() const
{
	return this->getROI().getBox(this->get_rMd().inv());
//	return transform(this->get_rMd().inv(), this->getROI());
}

QString RegionOfInterestMetric::getAsSingleLineString() const
{
	return "bb";
}

RegionOfInterest RegionOfInterestMetric::getROI() const
{
	RegionOfInterest retval;

	retval.mMargin = mMargin;

	if (mUseActiveTooltip)
	{
		retval.mPoints.push_back(this->getToolTip_r());
	}

	std::map<QString, DataPtr> alldata = mDataManager->getData();
	for (std::map<QString, DataPtr>::const_iterator i=alldata.begin(); i!=alldata.end(); ++i)
	{
		if (boost::dynamic_pointer_cast<RegionOfInterestMetric>(i->second))
			continue;

		if (mContainedData.contains(i->first))
		{
			std::vector<Vector3D> c = this->getCorners_r(i->second);
			std::copy(c.begin(), c.end(), back_inserter(retval.mPoints));
		}

		if (mMaxBoundsData == i->first)
		{
			retval.mMaxBoundsPoints = this->getCorners_r(i->second);
		}
	}

	return retval;
}

Vector3D RegionOfInterestMetric::getToolTip_r() const
{
	Transform3D rMto = mSpaceProvider->get_toMfrom(CoordinateSystem(csTOOL_OFFSET, "active"),
												   CoordinateSystem::reference());
	Vector3D tp = rMto.coord(Vector3D(0, 0, 0));
	return tp;
}

//DoubleBoundingBox3D RegionOfInterestMetric::getBasicROI() const
//{
//	// create a dummy ROI containing vol center and tool plus margin
//	std::map<QString, DataPtr> alldata = mDataManager->getData();
//	std::map<QString, DataPtr> data;

//	for (std::map<QString, DataPtr>::const_iterator i=alldata.begin(); i!=alldata.end(); ++i)
//		if (mContainedData.contains(i->first))
//			data[i->first] = i->second;

//	std::vector<Vector3D> points = getCorners_r_FromNonROI(data);

//	if (mUseActiveTooltip)
//	{
//		Transform3D rMto = mSpaceProvider->get_toMfrom(CoordinateSystem(csTOOL_OFFSET, "active"),
//													 CoordinateSystem::reference());
//		Vector3D tp = rMto.coord(Vector3D(0, 0, 0));
//		points.push_back(tp);
//	}

//	DoubleBoundingBox3D bb = this->generateROIFromPointsAndMargin(points, mMargin);
//	return bb;
//}

//DoubleBoundingBox3D RegionOfInterestMetric::getMaxROI() const
//{
//	std::map<QString, DataPtr> data;
//	if (!mDataManager->getData(mMaxBoundsData))
//		return DoubleBoundingBox3D::zero();
//	data[mMaxBoundsData] = mDataManager->getData(mMaxBoundsData);

//	std::vector<Vector3D> points = getCorners_r_FromNonROI(data);

//	DoubleBoundingBox3D bb = this->generateROIFromPointsAndMargin(points, mMargin);
//	return bb;
//}

//void RegionOfInterestMetric::getCorners_r_FromNonROI(std::vector<Vector3D>* sink, DataPtr data) const
//{
//	std::vector<Vector3D> points;
//	// create a max ROI containing all data plus margin
//	for (std::map<QString, DataPtr>::const_iterator i=data.begin(); i!=data.end(); ++i)
//	{
//		DataPtr current = i->second;
//		if (boost::dynamic_pointer_cast<RegionOfInterestMetric>(current))
//			continue;
//		std::vector<Vector3D> c = this->getCorners_r(current);
//		std::copy(c.begin(), c.end(), back_inserter(points));
//	}

//	return points;
//}

//std::vector<Vector3D> RegionOfInterestMetric::getCorners_r_FromNonROI(std::map<QString, DataPtr> data) const
//{
//	std::vector<Vector3D> points;
//	// create a max ROI containing all data plus margin
//	for (std::map<QString, DataPtr>::const_iterator i=data.begin(); i!=data.end(); ++i)
//	{
//		DataPtr current = i->second;
//		if (boost::dynamic_pointer_cast<RegionOfInterestMetric>(current))
//			continue;
//		std::vector<Vector3D> c = this->getCorners_r(current);
//		std::copy(c.begin(), c.end(), back_inserter(points));
//	}

//	return points;
//}

//DoubleBoundingBox3D RegionOfInterestMetric::generateROIFromPointsAndMargin(const std::vector<Vector3D>& points, double margin) const
//{
//	DoubleBoundingBox3D bb = DoubleBoundingBox3D::fromCloud(points);
//	Vector3D vmargin(margin,margin, margin);
//	Vector3D bl = bb.bottomLeft() - vmargin;
//	Vector3D tr = bb.topRight() + vmargin;
//	bb = DoubleBoundingBox3D(bl, tr);

//	return bb;
//}

std::vector<Vector3D> RegionOfInterestMetric::getCorners_r(DataPtr data) const
{
	DoubleBoundingBox3D bb = data->boundingBox();
	std::vector<Vector3D> retval;

	for (unsigned x=0; x<2; ++x)
		for (unsigned y=0; y<2; ++y)
			for (unsigned z=0; z<2; ++z)
				retval.push_back(bb.corner(x,y,z));

	for (unsigned i=0; i<retval.size(); ++i)
		retval[i] = data->get_rMd().coord(retval[i]);

	return retval;
}

}

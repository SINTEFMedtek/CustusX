/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
}

DoubleBoundingBox3D RegionOfInterestMetric::boundingBox() const
{
	return this->getROI().getBox(this->get_rMd().inv());
}

QString RegionOfInterestMetric::getAsSingleLineString() const
{
	return "bb";
}

template<class ITER>
void transform_coord_range(ITER begin, ITER end, Transform3D M)
{
	for ( ; begin!=end; ++begin)
		*begin = M.coord(*begin);
}

RegionOfInterest RegionOfInterestMetric::getROI() const
{
	RegionOfInterest retval;

	retval.mMargin = mMargin;

	if (mUseActiveTooltip)
	{
		retval.mPoints.push_back(this->getToolTip_r());
	}

	std::map<QString, DataPtr> alldata = mDataManager->getDatas();
	for (std::map<QString, DataPtr>::const_iterator i=alldata.begin(); i!=alldata.end(); ++i)
	{
		if (boost::dynamic_pointer_cast<RegionOfInterestMetric>(i->second))
			continue;

		if (mContainedData.contains(i->first))
		{
			std::vector<Vector3D> c = i->second->getPointCloud();
			transform_coord_range(c.begin(), c.end(), i->second->get_rMd());
			std::copy(c.begin(), c.end(), back_inserter(retval.mPoints));
		}

		if (mMaxBoundsData == i->first)
		{
			std::vector<Vector3D> c = i->second->getPointCloud();
			transform_coord_range(c.begin(), c.end(), i->second->get_rMd());
			retval.mMaxBoundsPoints = c;
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

}

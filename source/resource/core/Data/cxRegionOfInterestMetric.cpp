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

RegionOfInterestMetric::RegionOfInterestMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mUseActiveTooltip = false;
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
	return this->getROI() != DoubleBoundingBox3D::zero();
}

void RegionOfInterestMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	XMLNodeAdder adder(dataNode);
	for (unsigned i=0; i<mContainedData.size(); ++i)
		adder.addTextToElement("content", mContainedData[i]);

	adder.addTextToElement("useActiveTooltip", QString::number(mUseActiveTooltip));
}

void RegionOfInterestMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	XMLNodeParser parser(dataNode);
	mContainedData = parser.parseTextFromDuplicateElements("content");
	mUseActiveTooltip = parser.parseTextFromElement("useActiveTooltip").toInt();
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
	return transform(this->get_rMd().inv(), this->getROI());
}

QString RegionOfInterestMetric::getAsSingleLineString() const
{
	return "bb";
}


DoubleBoundingBox3D RegionOfInterestMetric::getROI() const
{
	// create a dummy ROI containing vol center and tool plus margin
	std::map<QString, DataPtr> alldata = mDataManager->getData();

	std::vector<Vector3D> points;
	// create a max ROI containing all data plus margin
	for (std::map<QString, DataPtr>::const_iterator i=alldata.begin(); i!=alldata.end(); ++i)
	{
		if (!mContainedData.contains(i->first))
			continue;
		std::vector<Vector3D> c = this->getCorners_r(i->second);
		std::copy(c.begin(), c.end(), back_inserter(points));
	}

	if (mUseActiveTooltip)
	{
		Transform3D rMto = mSpaceProvider->get_toMfrom(CoordinateSystem(csTOOL_OFFSET, "active"),
													 CoordinateSystem::reference());

		Vector3D tp = rMto.coord(Vector3D(0, 0, 0));
		points.push_back(tp);
	}

	double margin = 20;
	DoubleBoundingBox3D bb = this->generateROIFromPointsAndMargin(points, margin);
	return bb;
}

DoubleBoundingBox3D RegionOfInterestMetric::getMaxROI() const
{
	std::map<QString, DataPtr> alldata = mDataManager->getData();
	if (alldata.empty())
		return DoubleBoundingBox3D::zero();
	std::vector<Vector3D> points;
	// create a max ROI containing all data plus margin
	for (std::map<QString, DataPtr>::const_iterator i=alldata.begin(); i!=alldata.end(); ++i)
	{
		std::vector<Vector3D> c = this->getCorners_r(i->second);
		std::copy(c.begin(), c.end(), back_inserter(points));
	}

	double margin = 20;
	DoubleBoundingBox3D bb = this->generateROIFromPointsAndMargin(points, margin);
	return bb;
}

DoubleBoundingBox3D RegionOfInterestMetric::generateROIFromPointsAndMargin(const std::vector<Vector3D>& points, double margin) const
{
	DoubleBoundingBox3D bb = DoubleBoundingBox3D::fromCloud(points);
	Vector3D vmargin(margin,margin, margin);
	Vector3D bl = bb.bottomLeft() - vmargin;
	Vector3D tr = bb.topRight() + vmargin;
	bb = DoubleBoundingBox3D(bl, tr);

	return bb;
}

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

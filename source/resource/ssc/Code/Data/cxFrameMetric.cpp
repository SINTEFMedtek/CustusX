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

#include "cxFrameMetric.h"
#include "sscBoundingBox3D.h"
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{

ssc::DataPtr FrameMetricReader::load(const QString& uid, const QString& filename)
{
	return ssc::DataPtr(new FrameMetric(uid, filename));
}

FrameMetricPtr FrameMetric::create(QString uid, QString name)
{
    return FrameMetricPtr(new FrameMetric(uid, name));
}

FrameMetricPtr FrameMetric::create(QDomNode node)
{
    FrameMetricPtr retval = FrameMetric::create("");
    retval->parseXml(node);
    return retval;
}

FrameMetric::FrameMetric(const QString& uid, const QString& name) :
		ssc::DataMetric(uid, name),
		mSpace(ssc::SpaceHelpers::getR()),
		mFrame(ssc::Transform3D::Identity())
{
	mSpaceListener.reset(new ssc::CoordinateSystemListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

FrameMetric::~FrameMetric()
{
}

void FrameMetric::setFrame(const ssc::Transform3D& rMt)
{
	mFrame = rMt;
	emit transformChanged();
}

ssc::Transform3D FrameMetric::getFrame()
{
	return mFrame;
}

ssc::Vector3D FrameMetric::getCoordinate() const
{
	ssc::Vector3D point_t = ssc::Vector3D(0,0,0);
	return mFrame.coord(point_t);
}

/** return frame described in ref space F * sMr
  */
ssc::Transform3D FrameMetric::getRefFrame() const
{
    ssc::Transform3D rMq = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
    return rMq * mFrame;
}

/** return frame described in ref space F * sMr
  */
ssc::Vector3D FrameMetric::getRefCoord() const
{
    ssc::Transform3D rMq = this->getRefFrame();
//    ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
    ssc::Vector3D p_r = rMq.coord(ssc::Vector3D(0,0,0));
    return p_r;
}

void FrameMetric::setSpace(ssc::CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	ssc::Transform3D new_M_old = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), space);
	mFrame = new_M_old*mFrame;

	mSpace = space;
	mSpaceListener->setSpace(space);
}

ssc::CoordinateSystem FrameMetric::getSpace() const
{
	return mSpace;
}

void FrameMetric::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);

	dataNode.toElement().setAttribute("space", mSpace.toString());
	dataNode.toElement().setAttribute("frame", qstring_cast(mFrame));
}

void FrameMetric::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	this->setSpace(ssc::CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString())));
	this->setFrame(ssc::Transform3D::fromString(dataNode.toElement().attribute("frame", qstring_cast(mFrame))));
}

ssc::DoubleBoundingBox3D FrameMetric::boundingBox() const
{
	// convert both inputs to r space
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(this->getCoordinate());

	return ssc::DoubleBoundingBox3D(p0_r, p0_r);
}

QString FrameMetric::getAsSingleLineString()
{
	QString retval;
	retval += this->getType() + " \"";
	retval += mName + "\" \"";
	retval += mSpace.toString() + "\" ";
	retval += this->matrixAsSingleLineString();
	return retval;
}

QString FrameMetric::matrixAsSingleLineString()
{
	std::stringstream stream;
	mFrame.put(stream, 0, ' ');
	return qstring_cast(stream.str());
}

QString FrameMetric::pointAsSingleLineString()
{
	QString retval;
	QString elem;
	elem.setNum(mFrame.flatten()[3]);
	retval += elem + " ";
	elem.setNum(mFrame.flatten()[7]);
	retval += elem + " ";
	elem.setNum(mFrame.flatten()[11]);
	retval += elem;
	return retval;
}

} //namespace cx

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
	retval += this->getType() + " ";
	retval += mName + " ";
	retval += mSpace.toString() + " ";
	retval += this->matrixAsSingleLineString();
	return retval;
}

QString FrameMetric::matrixAsSingleLineString()
{
	QString retval;
	for(int i = 0; i < 16; ++i)
	{
		QString elem;
		elem.setNum(mFrame.flatten()[i]);
		retval += " ";
		retval += elem;
	}
	return retval;
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

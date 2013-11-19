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

#include "sscProbeData.h"
#include <QDomNode>
#include "sscTypeConversions.h"
#include <iostream>
#include "sscEnumConverter.h"
#include "sscMessageManager.h"

namespace cx
{

namespace
{
	double loadAttribute(QDomNode dataNode, QString name, double defVal)
	{
		QString text = dataNode.toElement().attribute(name);
		bool ok;
		double val = text.toDouble(&ok);
		if (ok)
			return val;
		return defVal;
	}
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


ProbeData::ProbeData(TYPE type) :
	mType(type), mDepthStart(0), mDepthEnd(0), mWidth(0),
	mTemporalCalibration(0), mCenterOffset(0), mSoundSpeedCompensationFactor(1.0),
	mUid("default"),
	mOrigin_p(0, 0, 0), mSpacing(-1, -1, -1), mClipRect_p(0, 0, 0, 0), mSize(0, 0)
{}

void ProbeData::addXml(QDomNode dataNode) const
{
	QDomElement elem = dataNode.toElement();
	elem.setAttribute("type", qstring_cast(mType));
	elem.setAttribute("depthStart", qstring_cast(mDepthStart));
	elem.setAttribute("depthEnd", qstring_cast(mDepthEnd));
	elem.setAttribute("width", qstring_cast(mWidth));

	elem.setAttribute("temporalCalibration", qstring_cast(mTemporalCalibration));
	elem.setAttribute("centerOffset", qstring_cast(mCenterOffset));

	elem.setAttribute("uid", qstring_cast(mUid));

	QDomElement imageNode = dataNode.ownerDocument().createElement("image");
	this->addImageXml(imageNode);
	dataNode.appendChild(imageNode);
}

void ProbeData::parseXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();

	mType = static_cast<TYPE>(elem.attribute("type").toInt());
	mDepthStart = loadAttribute(elem, "depthStart", 0);
	mDepthEnd = loadAttribute(elem, "depthEnd", 0);
	mWidth = loadAttribute(elem, "width", 0);

	mTemporalCalibration = loadAttribute(elem, "temporalCalibration", 0);
	mCenterOffset = loadAttribute(elem, "centerOffset", 0);
	mUid = elem.attribute("uid");

	QDomNode imageNode = dataNode.namedItem("image");
	this->parseImageXml(imageNode);
}

void ProbeData::addImageXml(QDomNode dataNode) const
{
	QDomElement elem = dataNode.toElement();

	elem.setAttribute("origin_p", qstring_cast(mOrigin_p));
	elem.setAttribute("spacing", qstring_cast(mSpacing));
	elem.setAttribute("clipRect_p", qstring_cast(mClipRect_p));
	elem.setAttribute("width", qstring_cast(mSize.width()));
	elem.setAttribute("height", qstring_cast(mSize.height()));
}

void ProbeData::parseImageXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();

	mOrigin_p = Vector3D::fromString(elem.attribute("origin_p"));
	mSpacing = Vector3D::fromString(elem.attribute("spacing"));
	mClipRect_p = DoubleBoundingBox3D::fromString(elem.attribute("clipRect_p"));
	mSize.setWidth(loadAttribute(elem, "width", 0));
	mSize.setHeight(loadAttribute(elem, "height", 0));
}

void ProbeData::setTemporalCalibration(double value)
{
	mTemporalCalibration = value;
}

void ProbeData::setType(TYPE type)
{
	mType = type;
}

void ProbeData::setSector(double depthStart, double depthEnd, double width, double centerOffset)
{
	mDepthStart=depthStart;
	mDepthEnd=depthEnd;
	mWidth=width;
	mCenterOffset=centerOffset;
}

ProbeData::TYPE ProbeData::getType() const
{
	return mType;
}

double ProbeData::getDepthStart() const
{
	return mDepthStart;
}

double ProbeData::getDepthEnd() const
{
	return mDepthEnd;
}

double ProbeData::getWidth() const
{
	return mWidth;
}

double ProbeData::getTemporalCalibration() const
{
	return mTemporalCalibration;
}

double ProbeData::getCenterOffset() const
{
	return mCenterOffset;
}

void ProbeData::resample(QSize newSize)
{
	if (newSize==mSize)
		return;

	Vector3D factor(double(newSize.width())/mSize.width(), double(newSize.height())/mSize.height(), 1);

	mOrigin_p = multiply_elems(mOrigin_p, factor);
	mSpacing = divide_elems(mSpacing, factor);

	Vector3D cr0 = multiply_elems(mClipRect_p.corner(0,0,0), factor);
	Vector3D cr1 = multiply_elems(mClipRect_p.corner(1,1,1), factor);
	mClipRect_p = DoubleBoundingBox3D(cr0, cr1);

	mSize = newSize;
}

QString ProbeData::getUid() const
{
	return mUid;
}

void ProbeData::setUid(QString uid)
{
	mUid = uid;
}

void ProbeData::updateClipRectFromSector()
{
	// cliprect and sector data are connected to linear probes:
	if (mType==tLINEAR)
	{
		mClipRect_p[0] = mOrigin_p[0] - mWidth/2/mSpacing[0];
		mClipRect_p[1] = mOrigin_p[0] + mWidth/2/mSpacing[0];

		mClipRect_p[2] = mOrigin_p[1] + mDepthStart/mSpacing[1];
		mClipRect_p[3] = mOrigin_p[1] + mDepthEnd/mSpacing[1];
	}
}

void ProbeData::updateSectorFromClipRect()
{
	// cliprect and sector data are connected to linear probes:
	if (mType==tLINEAR)
	{
		mWidth = 2*std::max(fabs(mClipRect_p[0] - mOrigin_p[0]), fabs(mClipRect_p[1] - mOrigin_p[0])) * mSpacing[0];
		mDepthStart = (mClipRect_p[2] - mOrigin_p[1]) * mSpacing[1];
		mDepthEnd = (mClipRect_p[3] - mOrigin_p[1]) * mSpacing[1];
	}
}

void ProbeData::applySoundSpeedCompensationFactor(double factor)
{
	mSpacing[1] = mSpacing[1] * factor / mSoundSpeedCompensationFactor;
	mSoundSpeedCompensationFactor = factor;

	if (this->getType() != ProbeData::tLINEAR)
		messageManager()->sendWarning("Sound speed compensation is applied to spacing[1], i.e. it is correct for linear probes and approxomate for other probes. Factor: " + qstring_cast(factor));
}

//Should be transform_uMv(p) (input in pixels, output in mm)
Vector3D ProbeData::transform_p_to_u(const Vector3D& q_p) const
{
	Vector3D c(q_p[0], double(mSize.height()) - q_p[1] - 1, -q_p[2]);
	c = multiply_elems(c, mSpacing);
	return c;
}

Vector3D ProbeData::getOrigin_u() const
{
	return this->transform_p_to_u(mOrigin_p);
}

DoubleBoundingBox3D ProbeData::getClipRect_u() const
{
		Vector3D p0 = transform_p_to_u(mClipRect_p.corner(0,0,0));
		Vector3D p1 = transform_p_to_u(mClipRect_p.corner(1,1,1));
		return DoubleBoundingBox3D(p0,p1);
}

Vector3D ProbeData::getOrigin_p() const
{
	return mOrigin_p;
}

Vector3D ProbeData::getSpacing() const
{
	return mSpacing;
}

DoubleBoundingBox3D ProbeData::getClipRect_p() const
{
	return mClipRect_p;
}

QSize ProbeData::getSize() const
{
	return mSize;
}

void ProbeData::setOrigin_p(Vector3D origin_p)
{
	mOrigin_p = origin_p;
}

void ProbeData::setSpacing(Vector3D spacing)
{
	mSpacing = spacing;
}

void ProbeData::setClipRect_p(DoubleBoundingBox3D clipRect_p)
{
	mClipRect_p = clipRect_p;
}

void ProbeData::setSize(QSize size)
{
	mSize = size;
}

}

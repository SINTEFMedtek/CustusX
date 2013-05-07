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

namespace ssc
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

ProbeData::ProbeImageData::ProbeImageData() :
	mOrigin_p(0, 0, 0), mSpacing(-1, -1, -1), mClipRect_p(0, 0, 0, 0), mSize(0, 0)
{
}

void ProbeData::ProbeImageData::addXml(QDomNode dataNode) const
{
	QDomElement elem = dataNode.toElement();

	elem.setAttribute("origin_p", qstring_cast(mOrigin_p));
	elem.setAttribute("spacing", qstring_cast(mSpacing));
	elem.setAttribute("clipRect_p", qstring_cast(mClipRect_p));
	elem.setAttribute("width", qstring_cast(mSize.width()));
	elem.setAttribute("height", qstring_cast(mSize.height()));
}

void ProbeData::ProbeImageData::parseXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();

	mOrigin_p = ssc::Vector3D::fromString(elem.attribute("origin_p"));
	mSpacing = ssc::Vector3D::fromString(elem.attribute("spacing"));
	mClipRect_p = ssc::DoubleBoundingBox3D::fromString(elem.attribute("clipRect_p"));
	mSize.setWidth(loadAttribute(elem, "width", 0));
	mSize.setHeight(loadAttribute(elem, "height", 0));
}


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------



ProbeData::ProbeData(TYPE type) :
	mType(type), mDepthStart(0), mDepthEnd(0), mWidth(0),
	mTemporalCalibration(0), mCenterOffset(0), mSoundSpeedCompensationFactor(1.0),
	mUid("default")
{
}


Vector3D ProbeData::ProbeImageData::transform_p_to_u(const Vector3D& q_p) const
{
	ssc::Vector3D c(q_p[0], double(mSize.height()) - q_p[1] - 1, -q_p[2]);
	c = multiply_elems(c, mSpacing);
	return c;
}

Vector3D ProbeData::ProbeImageData::getOrigin_u() const
{
	return this->transform_p_to_u(mOrigin_p);
}

ssc::DoubleBoundingBox3D ProbeData::ProbeImageData::getClipRect_u() const
{
	ssc::Vector3D p0 = transform_p_to_u(mClipRect_p.corner(0,0,0));
	ssc::Vector3D p1 = transform_p_to_u(mClipRect_p.corner(1,1,1));
	return ssc::DoubleBoundingBox3D(p0,p1);
}

void ProbeData::setImage(ProbeImageData value)
{
	mImage = value;
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

void ProbeData::updateClipRectFromSector()
{
	// cliprect and sector data are connected to linear probes:
	if (mType==tLINEAR)
	{
		mImage.mClipRect_p[0] = mImage.mOrigin_p[0] - mWidth/2/mImage.mSpacing[0];
		mImage.mClipRect_p[1] = mImage.mOrigin_p[0] + mWidth/2/mImage.mSpacing[0];

		mImage.mClipRect_p[2] = mImage.mOrigin_p[1] + mDepthStart/mImage.mSpacing[1];
		mImage.mClipRect_p[3] = mImage.mOrigin_p[1] + mDepthEnd/mImage.mSpacing[1];
	}
}

void ProbeData::updateSectorFromClipRect()
{
	// cliprect and sector data are connected to linear probes:
	if (mType==tLINEAR)
	{
		mWidth = 2*std::max(fabs(mImage.mClipRect_p[0] - mImage.mOrigin_p[0]), fabs(mImage.mClipRect_p[1] - mImage.mOrigin_p[0])) * mImage.mSpacing[0];
		mDepthStart = (mImage.mClipRect_p[2] - mImage.mOrigin_p[1]) * mImage.mSpacing[1];
		mDepthEnd = (mImage.mClipRect_p[3] - mImage.mOrigin_p[1]) * mImage.mSpacing[1];
	}
}

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
	mImage.addXml(imageNode);
	dataNode.appendChild(imageNode);
}

void ProbeData::parseXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();

//	mType = string2enum<TYPE>(elem.attribute("type"));
	mType = static_cast<TYPE>(elem.attribute("type").toInt());
	mDepthStart = loadAttribute(elem, "depthStart", 0);
	mDepthEnd = loadAttribute(elem, "depthEnd", 0);
	mWidth = loadAttribute(elem, "width", 0);

	mTemporalCalibration = loadAttribute(elem, "temporalCalibration", 0);
	mCenterOffset = loadAttribute(elem, "centerOffset", 0);
//	mUid = loadAttribute(elem, "uid", "default");
	mUid = elem.attribute("uid");


	QDomNode imageNode = dataNode.namedItem("image");
	mImage.parseXml(imageNode);
}

void ProbeData::setUid(QString uid)
{
	mUid = uid;
}

QString ProbeData::getUid()
{
	return mUid;
}

void ProbeData::applySoundSpeedCompensationFactor(double factor)
{

	mImage.mSpacing[1] = mImage.mSpacing[1] * factor / mSoundSpeedCompensationFactor;
	mSoundSpeedCompensationFactor = factor;

	if (this->getType() != ssc::ProbeData::tLINEAR)
	{
		ssc::messageManager()->sendWarning("Sound speed compensation is applied to spacing[1], i.e. it is correct for linear probes and approxomate for other probes. Factor: " + qstring_cast(factor));
	}
}

/**Set a new image size. resample all other parameters to match this new
 * image size, keeping sizes in millimeters fixed.
 *
 */
void ProbeData::resample(QSize newSize)
{
	if (newSize==mImage.mSize)
		return;

	ssc::Vector3D factor(double(newSize.width())/mImage.mSize.width(), double(newSize.height())/mImage.mSize.height(), 1);

	mImage.mOrigin_p = multiply_elems(mImage.mOrigin_p, factor);
	mImage.mSpacing = divide_elems(mImage.mSpacing, factor);

	ssc::Vector3D cr0 = multiply_elems(mImage.mClipRect_p.corner(0,0,0), factor);
	ssc::Vector3D cr1 = multiply_elems(mImage.mClipRect_p.corner(1,1,1), factor);
	mImage.mClipRect_p = ssc::DoubleBoundingBox3D(cr0, cr1);

	mImage.mSize = newSize;
}


}

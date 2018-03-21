/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxProbeDefinition.h"
#include <QDomNode>
#include "cxTypeConversions.h"
#include <iostream>
#include "cxEnumConverter.h"
#include "cxLogger.h"

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


ProbeDefinition::ProbeDefinition(TYPE type) :
	mType(type), mDepthStart(0), mDepthEnd(0), mWidth(0),
	mTemporalCalibration(0), mCenterOffset(0), mSoundSpeedCompensationFactor(1.0),
	mUid("default"),
	mOrigin_p(0, 0, 0), mSpacing(-1, -1, -1), mClipRect_p(0, 0, 0, 0), mSize(0, 0),
	mDigital(false)
{}

void ProbeDefinition::addXml(QDomNode dataNode) const
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

void ProbeDefinition::parseXml(QDomNode dataNode)
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

void ProbeDefinition::addImageXml(QDomNode dataNode) const
{
	QDomElement elem = dataNode.toElement();

	elem.setAttribute("origin_p", qstring_cast(mOrigin_p));
	elem.setAttribute("spacing", qstring_cast(mSpacing));
	elem.setAttribute("clipRect_p", qstring_cast(mClipRect_p));
	elem.setAttribute("width", qstring_cast(mSize.width()));
	elem.setAttribute("height", qstring_cast(mSize.height()));
}

void ProbeDefinition::parseImageXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();

	mOrigin_p = Vector3D::fromString(elem.attribute("origin_p"));
	mSpacing = Vector3D::fromString(elem.attribute("spacing"));
	mClipRect_p = DoubleBoundingBox3D::fromString(elem.attribute("clipRect_p"));
	mSize.setWidth(loadAttribute(elem, "width", 0));
	mSize.setHeight(loadAttribute(elem, "height", 0));
}

void ProbeDefinition::setTemporalCalibration(double value)
{
	mTemporalCalibration = value;
}

void ProbeDefinition::setType(TYPE type)
{
	mType = type;
}

void ProbeDefinition::setSector(double depthStart, double depthEnd, double width, double centerOffset)
{
	mDepthStart=depthStart;
	mDepthEnd=depthEnd;
	mWidth=width;
	mCenterOffset=centerOffset;
}

ProbeDefinition::TYPE ProbeDefinition::getType() const
{
	return mType;
}

double ProbeDefinition::getDepthStart() const
{
	return mDepthStart;
}

double ProbeDefinition::getDepthEnd() const
{
	return mDepthEnd;
}

double ProbeDefinition::getWidth() const
{
	return mWidth;
}

double ProbeDefinition::getTemporalCalibration() const
{
	return mTemporalCalibration;
}

double ProbeDefinition::getCenterOffset() const
{
	return mCenterOffset;
}

void ProbeDefinition::resample(QSize newSize)
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

QString ProbeDefinition::getUid() const
{
	return mUid;
}

void ProbeDefinition::setUid(QString uid)
{
	mUid = uid;
}

void ProbeDefinition::updateClipRectFromSector()
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

void ProbeDefinition::updateSectorFromClipRect()
{
	// cliprect and sector data are connected to linear probes:
	if (mType==tLINEAR)
	{
		mWidth = 2*std::max(fabs(mClipRect_p[0] - mOrigin_p[0]), fabs(mClipRect_p[1] - mOrigin_p[0])) * mSpacing[0];
		mDepthStart = (mClipRect_p[2] - mOrigin_p[1]) * mSpacing[1];
		mDepthEnd = (mClipRect_p[3] - mOrigin_p[1]) * mSpacing[1];
	}
}

void ProbeDefinition::applySoundSpeedCompensationFactor(double factor)
{
	if(similar(factor, 1.0))
		return;
	mSpacing[1] = mSpacing[1] * factor / mSoundSpeedCompensationFactor;
	mSoundSpeedCompensationFactor = factor;

	if (this->getType() != ProbeDefinition::tLINEAR)
		reportWarning("Sound speed compensation is applied to spacing[1], i.e. it is correct for linear probes and approxomate for other probes. Factor: " + qstring_cast(factor));
}

double ProbeDefinition::getSoundSpeedCompensationFactor() const
{
    return mSoundSpeedCompensationFactor;
}

//Should be transform_uMv(p) (input in pixels, output in mm)
Vector3D ProbeDefinition::transform_p_to_u(const Vector3D& q_p) const
{
	Vector3D c(q_p[0], double(mSize.height()) - q_p[1] - 1, -q_p[2]);
	c = multiply_elems(c, mSpacing);
	return c;
}

Vector3D ProbeDefinition::getOrigin_u() const
{
	return this->transform_p_to_u(mOrigin_p);
}

DoubleBoundingBox3D ProbeDefinition::getClipRect_u() const
{
		Vector3D p0 = transform_p_to_u(mClipRect_p.corner(0,0,0));
		Vector3D p1 = transform_p_to_u(mClipRect_p.corner(1,1,1));
		return DoubleBoundingBox3D(p0,p1);
}

Vector3D ProbeDefinition::getOrigin_p() const
{
	return mOrigin_p;
}

Vector3D ProbeDefinition::getSpacing() const
{
	return mSpacing;
}

DoubleBoundingBox3D ProbeDefinition::getClipRect_p() const
{
	return mClipRect_p;
}

QSize ProbeDefinition::getSize() const
{
	return mSize;
}

void ProbeDefinition::setOrigin_p(Vector3D origin_p)
{
	mOrigin_p = origin_p;
}

void ProbeDefinition::setSpacing(Vector3D spacing)
{
	mSpacing = spacing;
}

void ProbeDefinition::setClipRect_p(DoubleBoundingBox3D clipRect_p)
{
	mClipRect_p = clipRect_p;
}

void ProbeDefinition::setSize(QSize size)
{
	mSize = size;
}

void ProbeDefinition::setUseDigitalVideo(bool val)
{
	mDigital = val;
}

bool ProbeDefinition::getUseDigitalVideo() const
{
	return mDigital;
}

void ProbeDefinition::setVariable(QString variableName, QVariant value)
{
	mAdditionalVariables[variableName] = value;
}

QVariant ProbeDefinition::getVariable(QString variableName)
{
	if(!mAdditionalVariables.contains(variableName))
		CX_LOG_ERROR(QString("Variable %1 don't exist in ProbeDefinition with uid %2").arg(variableName).arg(this->mUid));
	return mAdditionalVariables[variableName];
}

} //cx

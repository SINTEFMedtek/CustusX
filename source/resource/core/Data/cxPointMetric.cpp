/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxPointMetric.h"
#include "cxBoundingBox3D.h"
#include "cxTool.h"
#include "cxTypeConversions.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxLogger.h"

namespace cx
{

PointMetric::PointMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
	DataMetric(uid, name, dataManager, spaceProvider),
	mCoordinate(0,0,0),
	mSpace(CoordinateSystem::reference())
{
	mSpaceListener = mSpaceProvider->createListener();
	mSpaceListener->setSpace(mSpace);
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SLOT(resetCachedValues()));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

PointMetricPtr PointMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return PointMetricPtr(new PointMetric(uid, name, dataManager, spaceProvider));
}

PointMetric::~PointMetric()
{
}

QString PointMetric::getParentSpace()
{
	return mSpaceProvider->convertToSpecific(mSpace).mRefObject;
}

void PointMetric::setCoordinate(const Vector3D& p)
{
	if (p == mCoordinate)
		return;

	mCoordinate = p;
	this->resetCachedValues();
	emit transformChanged();
}

Vector3D PointMetric::getCoordinate() const
{
	return mCoordinate;
}

void PointMetric::setSpace(CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	Transform3D new_M_old = mSpaceProvider->get_toMfrom(this->getSpace(), space);
	mCoordinate = new_M_old.coord(mCoordinate);

	mSpace = space;
	this->resetCachedValues();
	mSpaceListener->setSpace(space);
}

CoordinateSystem PointMetric::getSpace() const
{
	return mSpace;
}

void PointMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	dataNode.toElement().setAttribute("space", mSpace.toString());
	dataNode.toElement().setAttribute("coord", qstring_cast(mCoordinate));
}

void PointMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	this->setSpace(CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString())));
	this->setCoordinate(Vector3D::fromString(dataNode.toElement().attribute("coord", qstring_cast(mCoordinate))));
}

DoubleBoundingBox3D PointMetric::boundingBox() const
{
	// convert both inputs to r space
	Vector3D p0_r = this->getRefCoord();

	return DoubleBoundingBox3D(p0_r, p0_r);
}

void PointMetric::resetCachedValues()
{
	mCachedRefCoord.reset();
}

/** Utility function: return the coordinate the the reference space.
 *
 */
Vector3D PointMetric::getRefCoord() const
{
	if (!mCachedRefCoord.isValid())
	{
		Transform3D rM1 = mSpaceProvider->get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
		Vector3D val = rM1.coord(this->getCoordinate());
		mCachedRefCoord.set(val);
	}
	return mCachedRefCoord.get();
}

QString PointMetric::getValueAsString() const
{
	return prettyFormat(this->getRefCoord(), 1, 3);
}


}

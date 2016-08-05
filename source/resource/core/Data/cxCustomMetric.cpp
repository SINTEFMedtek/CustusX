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

#include "cxCustomMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxTypeConversions.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"

namespace cx
{

CustomMetric::CustomMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
    mArguments->setValidArgumentTypes(QStringList() << "pointMetric" << "frameMetric");
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
    mDefineVectorUpMethod = mDefineVectorUpMethods.table;
    mSTLFile = "";
	mScaleToP1 = false;
	mOffsetFromP0 = 0.0;

//	mToolListener = spaceProvider->createListener();
//	mToolListener->setSpace(CoordinateSystem(csTOOL, "active"));
//	connect(mToolListener.get(), &SpaceListener::changed, this, &CustomMetric::transformChanged);

}

CustomMetric::DefineVectorUpMethods CustomMetric::getDefineVectorUpMethods() const
{
    return mDefineVectorUpMethods;
}

CustomMetricPtr CustomMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
    return CustomMetricPtr(new CustomMetric(uid, name, dataManager, spaceProvider));
}

CustomMetric::~CustomMetric()
{
}

void CustomMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	mArguments->addXml(dataNode);

	QDomElement elem = dataNode.toElement();
	elem.setAttribute("definevectorup", mDefineVectorUpMethod);
	elem.setAttribute("STLFile", mSTLFile);

	elem.setAttribute("scaleToP1", mScaleToP1);
	elem.setAttribute("offsetFromP0", mOffsetFromP0);
}

void CustomMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getData());

	QDomElement elem = dataNode.toElement();
	mDefineVectorUpMethod = elem.attribute("definevectorup", qstring_cast(mDefineVectorUpMethod));
	mSTLFile = elem.attribute("STLFile", qstring_cast(mSTLFile));
	mScaleToP1 = elem.attribute("scaleToP1", QString::number(mScaleToP1)).toInt();
	mOffsetFromP0 = elem.attribute("offsetFromP0", QString::number(mOffsetFromP0)).toDouble();
}

bool CustomMetric::isValid() const
{
	return !mArguments->getRefCoords().empty();
}

Vector3D CustomMetric::getRefCoord() const
{
	return mArguments->getRefCoords().front();
}

DoubleBoundingBox3D CustomMetric::boundingBox() const
{
	return DoubleBoundingBox3D::fromCloud(mArguments->getRefCoords());
}

Vector3D CustomMetric::getPosition() const
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.empty())
		return Vector3D::Zero();

	Vector3D p = coords[0];
	Vector3D dir = getDirection();

	double positionOffset = 10; // the object is moved this distance from p0 towards p1
	return p + dir*mOffsetFromP0; // the object is moved mOffsetFromP0 from p0 towards p1
}

Vector3D CustomMetric::getDirection() const
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.size()<2)
		return Vector3D::UnitZ();
	Vector3D diff = (coords[1]-coords[0]);
	if (similar(diff.length(), 0.0))
		return Vector3D(0,1,0);
    return diff.normal();
}

Vector3D CustomMetric::getVectorUp() const
{
    if(mDefineVectorUpMethod == mDefineVectorUpMethods.connectedFrameInP1)
    {
        std::vector<Transform3D> transforms = mArguments->getRefFrames();
        if (transforms.size()<2)
            return Vector3D::UnitZ();

        Transform3D rMframe = transforms[1];
        Vector3D upVector = rMframe.vector(Vector3D(-1,0,0));

        return upVector;
    }
    else
		return mDataManager->getOperatingTable().getVectorUp();
}

Vector3D CustomMetric::getScale(DoubleBoundingBox3D bounds) const
{
//	bounds.range();

	if (!mScaleToP1)
		return Vector3D::Ones();

//	Vector3D p_to = mSpaceProvider->getActiveToolTipPoint(CoordinateSystem::reference(), true);

	std::vector<Vector3D> coords = mArguments->getRefCoords();
	double height = (coords[1] - coords[0]).length();

	Vector3D pos = this->getPosition();
	Vector3D dir = this->getDirection();

//	Vector3D dir = (coords[1]-coords[0]).normal();
	double p0 = dot(pos, dir);
	double p1 = dot(coords[1], dir);
//	double p2 = dot(p_to, dir);
	height = p1 - p0;
	// experiment: use tool to reduce size of object
	// - in order to make the Multiguide shrink the funnel depth as it advances towards target.
//	height = std::min(p1, p2) - p0;
//	height/=3; // emphasis
//	std::cout << ""

//	double diameter = 10; // scale max diameter of object
//	Vector3D scale(diameter, height, diameter);
	Vector3D scale(1, height/bounds.range()[1], 1);
	return scale;
}

QString CustomMetric::getAsSingleLineString() const
{
	return QString("%1 %2")
			.arg(this->getSingleLineHeader())
			.arg(qstring_cast(""));
}

QString CustomMetric::getDefineVectorUpMethod() const
{
    return mDefineVectorUpMethod;
}

void CustomMetric::setDefineVectorUpMethod(QString defineVectorUpMethod)
{
    mDefineVectorUpMethod = defineVectorUpMethod;
}

void CustomMetric::setSTLFile(QString val)
{
    mSTLFile = val;
    emit propertiesChanged();
}

QString CustomMetric::getSTLFile() const
{
	return mSTLFile;
}

void CustomMetric::setScaleToP1(bool val)
{
	if (mScaleToP1 == val)
		return;
	mScaleToP1 = val;
	emit propertiesChanged();
}

bool CustomMetric::getScaleToP1() const
{
	return mScaleToP1;
}

void CustomMetric::setOffsetFromP0(double val)
{
	if (mOffsetFromP0 == val)
		return;
	mOffsetFromP0 = val;
	emit propertiesChanged();
}

double CustomMetric::getOffsetFromP0() const
{
	return mOffsetFromP0;
}

QStringList CustomMetric::DefineVectorUpMethods::getAvailableDefineVectorUpMethods() const
{
    QStringList retval;
    retval << table;
    retval << connectedFrameInP1;
    return retval;
}

std::map<QString, QString> CustomMetric::DefineVectorUpMethods::getAvailableDefineVectorUpMethodsDisplayNames() const
{
    std::map<QString, QString> names;
    names[table] = "The operating table";
    names[connectedFrameInP1] = "The connected frame in p1";
    return names;
}

}

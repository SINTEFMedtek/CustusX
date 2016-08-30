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
#include "cxMesh.h"

namespace cx
{

CustomMetric::CustomMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
    mArguments->setValidArgumentTypes(QStringList() << "pointMetric" << "frameMetric");
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
    mDefineVectorUpMethod = mDefineVectorUpMethods.table;
	mMeshUid = "";
	mScaleToP1 = false;
	mOffsetFromP0 = 0.0;
	mRepeatDistance = 0.0;
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
	elem.setAttribute("meshUid", mMeshUid);

	elem.setAttribute("scaleToP1", mScaleToP1);
	elem.setAttribute("offsetFromP0", mOffsetFromP0);
	elem.setAttribute("repeatDistance", mRepeatDistance);
}

void CustomMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getDatas());

	QDomElement elem = dataNode.toElement();
	mDefineVectorUpMethod = elem.attribute("definevectorup", qstring_cast(mDefineVectorUpMethod));
	mMeshUid = elem.attribute("meshUid", qstring_cast(mMeshUid));
	mScaleToP1 = elem.attribute("scaleToP1", QString::number(mScaleToP1)).toInt();
	mOffsetFromP0 = elem.attribute("offsetFromP0", QString::number(mOffsetFromP0)).toDouble();
	mRepeatDistance = elem.attribute("repeatDistance", QString::number(mRepeatDistance)).toDouble();
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

std::vector<Vector3D> CustomMetric::getPositions() const
{
	std::vector<Vector3D> retval;
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.size() < 2)
		return retval;

	Vector3D p0 = coords[0];
	Vector3D p1 = coords[1];
	Vector3D dir = getDirection();
	double fullDist = dot(dir, p1-p0);

	int reps = this->getRepeatCount();
	for (int i=0; i<reps; ++i)
	{
		double dist = mOffsetFromP0 + mRepeatDistance*i;
		Vector3D p = p0 + dir*dist;
		retval.push_back(p);
	}

	return retval;
}

int CustomMetric::getRepeatCount() const
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.size() < 2)
		return 0;

	Vector3D p0 = coords[0];
	Vector3D p1 = coords[1];
	Vector3D dir = getDirection();

	int reps = 1;
	if (!similar(mRepeatDistance, 0.0))
		reps = (dot(p1-p0, dir)-mOffsetFromP0)/mRepeatDistance + 1;
	reps = std::min(100, reps);
	reps = std::max(reps, 1);

	return reps;
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

Vector3D CustomMetric::getScale() const
{
	if (!mScaleToP1)
		return Vector3D::Ones();

	DoubleBoundingBox3D bounds = this->getMesh()->boundingBox();

	std::vector<Vector3D> coords = mArguments->getRefCoords();
	double height = (coords[1] - coords[0]).length();

	Vector3D dir = this->getDirection();
	double p0 = dot(coords[0], dir);
	double p1 = dot(coords[1], dir);

	height = p1 - p0;

	Vector3D scale(1,
				   height/bounds.range()[1],
			1);
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

void CustomMetric::setMeshUid(QString val)
{
	mMeshUid = val;
    emit propertiesChanged();
}

QString CustomMetric::getMeshUid() const
{
	return mMeshUid;
}

MeshPtr CustomMetric::getMesh() const
{
	return mDataManager->getData<Mesh>(mMeshUid);
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

void CustomMetric::setRepeatDistance(double val)
{
	if (mRepeatDistance == val)
		return;
	mRepeatDistance = val;
	emit propertiesChanged();
}

double CustomMetric::getRepeatDistance() const
{
	return mRepeatDistance;
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

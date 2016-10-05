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

#include <vtkImageData.h>
#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxTypeConversions.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"
#include "cxData.h"
#include "cxMesh.h"
#include "cxImage.h"

namespace cx
{

CustomMetric::CustomMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider), mShowDistanceMarkers(false), mDistanceMarkerVisibility(50)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
    mArguments->setValidArgumentTypes(QStringList() << "pointMetric" << "frameMetric");
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
    mDefineVectorUpMethod = mDefineVectorUpMethods.table;
	mModelUid = "";
	mScaleToP1 = false;
	mOffsetFromP0 = 0.0;
	mRepeatDistance = 0.0;
	mTranslationOnly = false;
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
	elem.setAttribute("meshUid", mModelUid);

	elem.setAttribute("scaleToP1", mScaleToP1);
	elem.setAttribute("offsetFromP0", mOffsetFromP0);
	elem.setAttribute("repeatDistance", mRepeatDistance);
	elem.setAttribute("showDistance", mShowDistanceMarkers);
	elem.setAttribute("translationOnly", mTranslationOnly);
}

void CustomMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getDatas());

	QDomElement elem = dataNode.toElement();
	mDefineVectorUpMethod = elem.attribute("definevectorup", qstring_cast(mDefineVectorUpMethod));
	mModelUid = elem.attribute("meshUid", qstring_cast(mModelUid));
	mScaleToP1 = elem.attribute("scaleToP1", QString::number(mScaleToP1)).toInt();
	mOffsetFromP0 = elem.attribute("offsetFromP0", QString::number(mOffsetFromP0)).toDouble();
	mRepeatDistance = elem.attribute("repeatDistance", QString::number(mRepeatDistance)).toDouble();
	mShowDistanceMarkers = elem.attribute("showDistance", QString::number(mShowDistanceMarkers)).toInt();
	mTranslationOnly = elem.attribute("translationOnly", QString::number(mTranslationOnly)).toInt();
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

std::vector<Vector3D> CustomMetric::getPointCloud() const
{
	std::vector<Vector3D> retval;

	DataPtr model = this->getModel();

	std::vector<Transform3D> pos = this->calculateOrientations();
	std::vector<Vector3D> cloud;
	Transform3D rrMd;

	if (model)
	{
		rrMd = model->get_rMd();
		cloud = model->getPointCloud();
	}
	else
	{
		cloud.push_back(Vector3D::Zero());
		rrMd = Transform3D::Identity();
	}

	for (unsigned i=0; i<pos.size(); ++i)
	{
		Transform3D rMd = pos[i] * rrMd;

		for (unsigned j=0; j<cloud.size(); ++j)
		{
			Vector3D p_r = rMd.coord(cloud[j]);
			retval.push_back(p_r);
		}
	}

	return retval;
}

Vector3D CustomMetric::getZeroPosition() const
{
	Vector3D zeroPos;
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.empty())
		return zeroPos;
	Vector3D p0 = coords[0];

	Vector3D dir = this->getDirection();
	Vector3D vup = this->getVectorUp();
	Vector3D scale = this->getScale();

	zeroPos = this->calculateOrientation(p0, dir, vup, scale).coord(Vector3D(0,0,0));

	return zeroPos;
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
	if (!mScaleToP1 || !this->getModel() || this->getModel()->getType() == "image")
		return Vector3D::Ones();

	DoubleBoundingBox3D bounds = this->getModel()->boundingBox();
	bounds = transform(this->getModel()->get_rMd(), bounds);

	std::vector<Vector3D> coords = mArguments->getRefCoords();
	double height = (coords[1] - coords[0]).length();

	Vector3D dir = this->getDirection();
	double p0 = dot(coords[0], dir);
	double p1 = dot(coords[1], dir);

	height = p1 - p0;
	height -= mOffsetFromP0;

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

void CustomMetric::setModelUid(QString val)
{
	mModelUid = val;
    emit propertiesChanged();
}

QString CustomMetric::getModelUid() const
{
	return mModelUid;
}

DataPtr CustomMetric::getModel() const
{
	return mDataManager->getData(mModelUid);
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

void CustomMetric::setShowDistanceMarkers(bool show)
{
	if(mShowDistanceMarkers == show)
		return;
	mShowDistanceMarkers = show;
	emit propertiesChanged();
}

bool CustomMetric::getShowDistanceMarkers() const
{
	return mShowDistanceMarkers;
}
	void CustomMetric::setTranslationOnly(bool val)
{
	if (mTranslationOnly == val)
		return;
	mTranslationOnly = val;
	emit propertiesChanged();
}

void CustomMetric::setDistanceMarkerVisibility(double val)
{
	if (mDistanceMarkerVisibility == val)
		return;
	mDistanceMarkerVisibility = val;
	emit propertiesChanged();
}

double CustomMetric::getDistanceMarkerVisibility() const
{
	return mDistanceMarkerVisibility;
}

bool CustomMetric::getTranslationOnly() const
{
	return mTranslationOnly;
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


std::vector<Transform3D> CustomMetric::calculateOrientations() const
{
	std::vector<Vector3D> pos = this->getPositions();
	Vector3D dir = this->getDirection();
	Vector3D vup = this->getVectorUp();
	Vector3D scale = this->getScale();

	std::vector<Transform3D> retval(pos.size());
	for (unsigned i=0; i<retval.size(); ++i)
	{
		if (mTranslationOnly)
		{
			retval[i] = createTransformTranslate(pos[i]);
		}
		else
		{
			retval[i] = this->calculateOrientation(pos[i], dir, vup, scale);
		}
	}

	return retval;
}

/**
 * Based on a position+direction, view up and scale,
 * calculate an orientation matrix combining these.
 */
Transform3D CustomMetric::calculateOrientation(Vector3D pos, Vector3D dir, Vector3D vup, Vector3D scale) const
{
	Transform3D R = this->calculateRotation(dir, vup);

	Transform3D center2DImage = this->calculateTransformTo2DImageCenter();

	Transform3D S = createTransformScale(scale);
	Transform3D T = createTransformTranslate(pos);
	Transform3D M = T*R*S*center2DImage;
	return M;
}

Transform3D CustomMetric::calculateTransformTo2DImageCenter() const
{
	Transform3D position2DImage = Transform3D::Identity();
	if(this->modelIsImage())
	{
		DataPtr model = this->getModel();
		ImagePtr imageModel = boost::dynamic_pointer_cast<Image>(model);
		vtkImageDataPtr vtkImage = imageModel->getBaseVtkImageData();
		Eigen::Array3i dimensions(vtkImage->GetDimensions());

		position2DImage = createTransformTranslate(Vector3D(-dimensions[0]/2, -dimensions[1]/2, 0));
	}
	return position2DImage;
}

bool CustomMetric::modelIsImage() const
{
	DataPtr model = this->getModel();

	return (model && model->getType() == "image");
}

Transform3D CustomMetric::calculateRotation(Vector3D dir, Vector3D vup) const
{
	Transform3D R = Transform3D::Identity();
	bool directionAlongUp = similar(dot(vup, dir.normal()), 1.0);
	if (!directionAlongUp)
	{
		Vector3D jvec = dir.normal();
		Vector3D kvec = cross(vup, dir).normal();
		Vector3D ivec = cross(jvec, kvec).normal();
		Vector3D center = Vector3D::Zero();
		R = createTransformIJC(ivec, jvec, center);

		Transform3D rotateY = cx::createTransformRotateY(M_PI_2);
		R = R*rotateY;//Let the models X-axis align with patient X-axis

		if(this->modelIsImage())
		{
			Transform3D rotateX = cx::createTransformRotateX(M_PI_2);
			R = R*rotateX;
		}
	}
	return R;
}

}

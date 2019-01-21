/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCustomMetric.h"

#include <vtkImageData.h>
#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxTypeConversions.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxLogger.h"
#include "cxFrameMetric.h"

namespace cx
{

CustomMetric::CustomMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider), mShowDistanceMarkers(false), mDistanceMarkerVisibility(50)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
		mArguments->setValidArgumentTypes(QStringList() << PointMetric::getTypeName() << FrameMetric::getTypeName());
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
	connect(this, &CustomMetric::propertiesChanged, this, &CustomMetric::onPropertiesChanged);
	mDefineVectorUpMethod = mDefineVectorUpMethods.table;
	mModelUid = "";
	mScaleToP1 = false;
	mOffsetFromP0 = 0.0;
	mOffsetFromP1 = 0.0;
	mRepeatDistance = 0.0;
	mTranslationOnly = false;
	mTextureFollowTool = false;
}

bool CustomMetric::needForToolListenerHasChanged() const
{
	bool toolDefinesUp = mDefineVectorUpMethod == mDefineVectorUpMethods.tool;
	bool toolListenerDefined = mToolListener.get()!=NULL;

	if (mTextureFollowTool != toolListenerDefined || toolDefinesUp != toolListenerDefined)
		return true;
	else
		return false;
}


void CustomMetric::createOrDestroyToolListener()
{
	if (this->needForToolListenerHasChanged())
	{
		bool toolDefinesUp = mDefineVectorUpMethod == mDefineVectorUpMethods.tool;
		if (mTextureFollowTool || toolDefinesUp)
		{
			mToolListener = mSpaceProvider->createListener();
			mToolListener->setSpace(CoordinateSystem(csTOOL_OFFSET, "active"));
			connect(mToolListener.get(), &SpaceListener::changed, this, &CustomMetric::transformChanged);
		}
		else
		{
			disconnect(mToolListener.get(), &SpaceListener::changed, this, &CustomMetric::transformChanged);
			mToolListener.reset();
		}
	}
}

void CustomMetric::onPropertiesChanged()
{
	this->createOrDestroyToolListener();
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
	elem.setAttribute("offsetFromP1", mOffsetFromP1);
	elem.setAttribute("repeatDistance", mRepeatDistance);
	elem.setAttribute("showDistance", mShowDistanceMarkers);
	elem.setAttribute("distanceMarkerVisibility", mDistanceMarkerVisibility);
	elem.setAttribute("translationOnly", mTranslationOnly);
	elem.setAttribute("textureFollowTool", mTextureFollowTool);
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
	mOffsetFromP1 = elem.attribute("offsetFromP1", QString::number(mOffsetFromP1)).toDouble();
	mRepeatDistance = elem.attribute("repeatDistance", QString::number(mRepeatDistance)).toDouble();
	mShowDistanceMarkers = elem.attribute("showDistance", QString::number(mShowDistanceMarkers)).toInt();
	mDistanceMarkerVisibility = elem.attribute("distanceMarkerVisibility", QString::number(mDistanceMarkerVisibility)).toDouble();
	mTranslationOnly = elem.attribute("translationOnly", QString::number(mTranslationOnly)).toInt();
	mTextureFollowTool = elem.attribute("textureFollowTool", QString::number(mTextureFollowTool)).toInt();

	this->onPropertiesChanged();
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
		reps = (dot(p1-p0, dir)-mOffsetFromP0-mOffsetFromP1)/mRepeatDistance + 1;
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
	else if (mDefineVectorUpMethod == mDefineVectorUpMethods.tool)
	{
		Transform3D rMt = mSpaceProvider->getActiveToolTipTransform(CoordinateSystem::reference(), true);
		Vector3D toolUp = -Vector3D::UnitX();
		return rMt.vector(toolUp);
	}
	else
	{
		return mDataManager->getOperatingTable().getVectorUp();
	}
}


void CustomMetric::updateTexture(MeshPtr model, Transform3D rMrr)
{
	if (!model)
		return;

	if (!this->getTextureFollowTool() || !model->hasTexture())
		return;

	// special case:
	// Project tool position down to the model, then set that position as
	// the texture x pos.

	Transform3D rMt = mSpaceProvider->getActiveToolTipTransform(CoordinateSystem::reference());
	Transform3D rMd = rMrr * model->get_rMd();
	Vector3D t_r = rMt.coord(Vector3D::Zero());
	Vector3D td_r = rMt.vector(Vector3D::UnitZ());

	DoubleBoundingBox3D bb_d = model->boundingBox();
	Vector3D bl = bb_d.bottomLeft();
	Vector3D tr = bb_d.topRight();
	Vector3D c = (bl+tr)/2;
	Vector3D x_min_r(c[0], bl[1], c[2]);
	Vector3D x_max_r(c[0], tr[1], c[2]);
	x_min_r = rMd.coord(x_min_r);
	x_max_r = rMd.coord(x_max_r);

	double t_x = dot(t_r, td_r);
	double bbmin_x = dot(x_min_r, td_r);
	double bbmax_x = dot(x_max_r, td_r);
	double range = bbmax_x-bbmin_x;
	if (similar(range, 0.0))
		range = 1.0E-6;
	double s = (t_x-bbmin_x)/range;
	model->getTextureData().getPositionY()->setValue(s);
}

Vector3D CustomMetric::getScale() const
{
	if (!mScaleToP1 || !this->getModel() || this->getModel()->getType() == Image::getTypeName())
		return Vector3D::Ones();

	DoubleBoundingBox3D bounds = this->getModel()->boundingBox();
	bounds = transform(this->getModel()->get_rMd(), bounds);

	std::vector<Vector3D> coords = mArguments->getRefCoords();
	double height = (coords[1] - coords[0]).length();

	Vector3D dir = this->getDirection();
	double p0 = dot(coords[0], dir);
	double p1 = dot(coords[1], dir);

	height = p1 - p0;
	height -= (mOffsetFromP0 + mOffsetFromP1);

	Vector3D scale(1,
				   height/bounds.range()[1],
			1);
	return scale;
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

void CustomMetric::setOffsetFromP1(double val)
{
	if (mOffsetFromP1 == val)
		return;
	mOffsetFromP1 = val;
	emit propertiesChanged();
}

double CustomMetric::getOffsetFromP1() const
{
	return mOffsetFromP1;
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

void CustomMetric::setTextureFollowTool(bool val)
{
	if (mTextureFollowTool == val)
		return;
	mTextureFollowTool = val;
	emit propertiesChanged();
}

bool CustomMetric::getTextureFollowTool() const
{
	return mTextureFollowTool;
}

QStringList CustomMetric::DefineVectorUpMethods::getAvailableDefineVectorUpMethods() const
{
    QStringList retval;
    retval << table;
    retval << connectedFrameInP1;
	retval << tool;
    return retval;
}

std::map<QString, QString> CustomMetric::DefineVectorUpMethods::getAvailableDefineVectorUpMethodsDisplayNames() const
{
    std::map<QString, QString> names;
    names[table] = "The operating table";
    names[connectedFrameInP1] = "The connected frame in p1";
	names[tool] = "The active tool";
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
		int xSize = vtkImage->GetExtent()[1] - vtkImage->GetExtent()[0];
		int ySize = vtkImage->GetExtent()[3] - vtkImage->GetExtent()[2];
		Eigen::Array3d spacing(vtkImage->GetSpacing());

		position2DImage = createTransformTranslate(Vector3D(-xSize*spacing[0]/2.0, -ySize*spacing[1]/2.0, 0));
	}
	return position2DImage;
}

bool CustomMetric::modelIsImage() const
{
	DataPtr model = this->getModel();

	return (model && model->getType() == Image::getTypeName());
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

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCustomMetricRep.h"

#include <boost/shared_ptr.hpp>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkSelectVisiblePoints.h>
#include "cxTypeConversions.h"
#include "cxCustomMetric.h"
#include "cxLogger.h"
#include "cxBoundingBox3D.h"
#include "cxGeometricRep.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxImage2DRep3D.h"
#include "cxGraphicalPrimitives.h"


namespace cx
{

CustomMetricRepPtr CustomMetricRep::New(const QString& uid)
{
    return wrap_new(new CustomMetricRep(), uid);
}

CustomMetricRep::CustomMetricRep()
{
}

void CustomMetricRep::clear()
{
	DataMetricRep::clear();
	mMeshGeometry.clear();
	for(int i = 0; i < mImageGeometryProxy.size(); ++i)
	{
		this->getRenderer()->RemoveActor(mImageGeometryProxy[i]->getActor());
	}
	mImageGeometryProxy.clear();
}

CustomMetricPtr CustomMetricRep::getCustomMetric()
{
    return boost::dynamic_pointer_cast<CustomMetric>(mMetric);
}

void CustomMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

	this->updateModel();
	this->drawText();
}

void CustomMetricRep::onEveryRender()
{
	this->hideDistanceMetrics();
}

void CustomMetricRep::updateModel()
{
	this->clear();
	CustomMetricPtr custom = this->getCustomMetric();

	if (!this->getView() || !custom)
	   return;

	DataPtr model = custom->getModel();

	if(custom->modelIsImage())
		this->updateImageModel(model);
	else
		this->updateMeshModel(model);
	this->createDistanceMarkers();
}

void CustomMetricRep::updateImageModel(DataPtr model)
{
	ImagePtr imageModel = boost::dynamic_pointer_cast<Image>(model);

	if(!imageModel && !imageModel->is2D())
		return;

	CustomMetricPtr custom = this->getCustomMetric();
	std::vector<Transform3D> pos = custom->calculateOrientations();

	mImageGeometryProxy.resize(pos.size());

	for(unsigned i = 0; i < mImageGeometryProxy.size(); ++i)
	{
		if(!mImageGeometryProxy[i])
			mImageGeometryProxy[i] = cx::Image2DProxy::New();

		mImageGeometryProxy[i]->setImage(imageModel);
		this->getRenderer()->AddActor(mImageGeometryProxy[i]->getActor());

		mImageGeometryProxy[i]->setTransformOffset(pos[i]);
	}
}

void CustomMetricRep::updateMeshModel(DataPtr model)
{
	MeshPtr meshModel = boost::dynamic_pointer_cast<Mesh>(model);

	CustomMetricPtr custom = this->getCustomMetric();
	std::vector<Transform3D> pos = custom->calculateOrientations();

	mMeshGeometry.resize(pos.size());

	for (unsigned i=0; i<mMeshGeometry.size(); ++i)
	{
		if (!mMeshGeometry[i])
		{
			mMeshGeometry[i].reset(new GraphicalGeometric);
			mMeshGeometry[i]->setRenderer(this->getRenderer());
		}

		mMeshGeometry[i]->setMesh(meshModel);

		mMeshGeometry[i]->setTransformOffset(pos[i]);

		custom->updateTexture(meshModel, pos[i]);
	}

}

void CustomMetricRep::createDistanceMarkers()
{
	mDistanceText.clear();
	CustomMetricPtr custom = this->getCustomMetric();
	if(!custom->getModel() || !custom->getShowDistanceMarkers())
		return;
	std::vector<Transform3D> pos = custom->calculateOrientations();

	if(pos.size() < 2)
		return;

	DoubleBoundingBox3D bounds = custom->getModel()->boundingBox();


	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(pos.size());

	mDistanceText.resize(pos.size());
	Vector3D pos_0 = custom->getZeroPosition();
	for(unsigned i = 0; i < mDistanceText.size(); ++i)
	{
		Vector3D pos_i = pos[i].coord(Vector3D(0,0,0));
		double distance = (pos_i - pos_0).length();
		Vector3D textpos = bounds.center();
		textpos[2] = bounds.bottomLeft()[2];
		mDistanceText[i] = this->createDistanceText(pos[i].coord(textpos), distance);

		Vector3D point = pos[i].coord(textpos);
		vtkIdType pointId = i;
		points->SetPoint(pointId, point.data());
	}
}

CaptionText3DPtr CustomMetricRep::createDistanceText(Vector3D pos, double distance)
{
	CaptionText3DPtr text = CaptionText3DPtr(new CaptionText3D(this->getRenderer()));
	text->setColor(mMetric->getColor());
	text->setText(QString("%1").arg(distance));

	text->setPosition(pos);
	text->placeAboveCenter();
	text->setSize(mLabelSize / 100);

	return text;
}

//Hide the distance metrics if outside the view port, obscured by other structures, or of too far from the camera
void CustomMetricRep::hideDistanceMetrics()
{
	if(mDistanceText.empty())
		return;
	static vtkSmartPointer<vtkSelectVisiblePoints> visPts = vtkSmartPointer<vtkSelectVisiblePoints>::New();
	visPts->SetRenderer(this->getRenderer());
	float * zbuffer = visPts->Initialize(true);

	for(unsigned i = 0; i < mDistanceText.size(); ++i)
	{
		Vector3D pos = mDistanceText[i]->getPosition();
		bool visible = visPts->IsPointOccluded(pos.data(), zbuffer);
		bool closeToCamera = this->isCloseToCamera(pos);
		mDistanceText[i]->setVisibility(visible && closeToCamera);
	}
	delete zbuffer;
}

bool CustomMetricRep::isCloseToCamera(Vector3D pos)
{
	double distanceThreshold = this->getCustomMetric()->getDistanceMarkerVisibility();
	Vector3D cameraPos(this->getRenderer()->GetActiveCamera()->GetPosition());
	Vector3D diff = cameraPos - pos;
	double distance = diff.norm();
	if(distance < distanceThreshold)
		return true;
	return false;
}

}

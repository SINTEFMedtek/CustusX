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

#include "cxCustomMetricRep.h"

#include <boost/shared_ptr.hpp>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkImageActor.h>
#include <vtkTextActor.h>
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "cxCustomMetric.h"
#include "vtkMatrix4x4.h"
#include "vtkSTLReader.h"
#include <vtkPolyDataNormals.h>
#include "cxLogger.h"
#include "cxBoundingBox3D.h"
#include "cxGeometricRep.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxImage2DRep3D.h"
#include "cxGraphicalPrimitives.h"

#include <vtkSelectVisiblePoints.h>
#include <vtkIdFilter.h>
#include <vtkLabeledDataMapper.h>

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


	//Used by createDistanceMarkersPipeline(). To be removed
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
	text->placeBelowCenter();//Test
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
		bool closeToCamera = this->isCloserToCameraThan(pos, 25);
		mDistanceText[i]->setVisibility(visible && closeToCamera);
	}
	delete zbuffer;
}

bool CustomMetricRep::isCloserToCameraThan(Vector3D pos, double distanceThreshold)
{
	Vector3D cameraPos(this->getRenderer()->GetActiveCamera()->GetPosition());
	Vector3D diff = cameraPos - pos;
	double distance = diff.norm();
	if(distance < distanceThreshold)
		return true;
	return false;
}

}

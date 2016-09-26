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

#include "cxView.h"

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
#include <vtkImageData.h>
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "cxGraphicalPrimitives.h"
#include "cxCustomMetric.h"
#include "cxGraphicalPrimitives.h"
#include "vtkMatrix4x4.h"
#include "vtkSTLReader.h"
#include <vtkPolyDataNormals.h>
#include "cxLogger.h"
#include "cxBoundingBox3D.h"
#include "cxGeometricRep.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxImage2DRep3D.h"

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

void CustomMetricRep::updateModel()
{
	this->clear();
	CustomMetricPtr custom = this->getCustomMetric();

	if (!this->getView() || !custom)
	   return;

	DataPtr model = custom->getModel();

	if(this->modelIsImage())
		this->updateImageModel(model);
	else
		this->updateMeshModel(model);
}

bool CustomMetricRep::modelIsImage()
{
	CustomMetricPtr custom = this->getCustomMetric();
	DataPtr model = custom->getModel();

	return (model && model->getType() == "image");
}

void CustomMetricRep::updateImageModel(DataPtr model)
{
	ImagePtr imageModel = boost::dynamic_pointer_cast<Image>(model);

	if(!imageModel && !imageModel->is2D())
		return;

	CustomMetricPtr custom = this->getCustomMetric();
	std::vector<Vector3D> pos = custom->getPositions();

	mImageGeometryProxy.resize(pos.size());

	for(unsigned i = 0; i < mImageGeometryProxy.size(); ++i)
	{
		if(!mImageGeometryProxy[i])
			mImageGeometryProxy[i] = cx::Image2DProxy::New();

		mImageGeometryProxy[i]->setImage(imageModel);
		this->getRenderer()->AddActor(mImageGeometryProxy[i]->getActor());

		Transform3D M = this->calculateOrientation(pos[i]);
		mImageGeometryProxy[i]->setTransformOffset(M);
	}
}

void CustomMetricRep::updateMeshModel(DataPtr model)
{
	MeshPtr meshModel = boost::dynamic_pointer_cast<Mesh>(model);

	CustomMetricPtr custom = this->getCustomMetric();
	std::vector<Vector3D> pos = custom->getPositions();

	mMeshGeometry.resize(pos.size());

	for (unsigned i=0; i<pos.size(); ++i)
	{
		if (!mMeshGeometry[i])
		{
			mMeshGeometry[i].reset(new GraphicalGeometric);
			mMeshGeometry[i]->setRenderer(this->getRenderer());
		}
		mMeshGeometry[i]->setMesh(meshModel);

		Transform3D M = this->calculateOrientation(pos[i]);
		mMeshGeometry[i]->setTransformOffset(M);
	}
}

Transform3D CustomMetricRep::calculateOrientation(Vector3D pos)
{
	CustomMetricPtr custom = this->getCustomMetric();
	Vector3D dir = custom->getDirection();
	Vector3D vup = custom->getVectorUp();
	Vector3D scale = custom->getScale();
	Transform3D M = this->calculateOrientation(pos, dir, vup, scale);
	return M;
}

/**
 * Based on a position+direction, view up and scale,
 * calculate an orientation matrix combining these.
 */
Transform3D CustomMetricRep::calculateOrientation(Vector3D pos, Vector3D dir, Vector3D vup, Vector3D scale)
{
	Transform3D R = this->calculateRotation(dir, vup);

	Transform3D center2DImage = this->calculateTransformTo2DImageCenter();

	Transform3D S = createTransformScale(scale);
	Transform3D T = createTransformTranslate(pos);
	Transform3D M = T*R*S*center2DImage;
	return M;
}

Transform3D CustomMetricRep::calculateTransformTo2DImageCenter()
{
	Transform3D position2DImage = Transform3D::Identity();
	if(this->modelIsImage())
	{
		CustomMetricPtr custom = this->getCustomMetric();
		DataPtr model = custom->getModel();
		ImagePtr imageModel = boost::dynamic_pointer_cast<Image>(model);
		vtkImageDataPtr vtkImage = imageModel->getBaseVtkImageData();
		Eigen::Array3i dimensions(vtkImage->GetDimensions());

		position2DImage = createTransformTranslate(Vector3D(-dimensions[0]/2, -dimensions[1]/2, 0));
	}
	return position2DImage;
}

Transform3D CustomMetricRep::calculateRotation(Vector3D dir, Vector3D vup)
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

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

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "vtkTextActor.h"
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
	mGeometry.clear();
}

CustomMetricPtr CustomMetricRep::getCustomMetric()
{
    return boost::dynamic_pointer_cast<CustomMetric>(mMetric);
}

void CustomMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

    this->updateSTLModel();
	this->drawText();
}

void CustomMetricRep::updateSTLModel()
{
	CustomMetricPtr custom = this->getCustomMetric();

	if (!this->getView() || !custom)
	   return;

	std::vector<Transform3D> pos = custom->calculateOrientations();

//	std::vector<Vector3D> pos = custom->getPositions();
//	Vector3D dir = custom->getDirection();
//	Vector3D vup = custom->getVectorUp();
//	Vector3D scale = custom->getScale();

	mGeometry.resize(pos.size());

	for (unsigned i=0; i<pos.size(); ++i)
	{
		if (!mGeometry[i])
		{
			mGeometry[i].reset(new GraphicalGeometric);
			mGeometry[i]->setRenderer(this->getRenderer());
		}
		mGeometry[i]->setMesh(custom->getMesh());

//		Transform3D M = this->calculateOrientation(pos[i], dir, vup, scale);
		mGeometry[i]->setTransformOffset(pos[i]);
	}
}

///**
// * Based on a position+direction, view up and scale,
// * calculate an orientation matrix combining these.
// */
//Transform3D CustomMetricRep::calculateOrientation(Vector3D pos, Vector3D dir, Vector3D vup, Vector3D scale)
//{
//	Transform3D R;
//	bool directionAlongUp = similar(dot(vup, dir.normal()), 1.0);

//	if (directionAlongUp)
//	{
//		R = Transform3D::Identity();
//	}
//	else
//	{
//		Vector3D jvec = dir.normal();
//		Vector3D kvec = cross(vup, dir).normal();
//		Vector3D ivec = cross(jvec, kvec).normal();
//		Vector3D center = Vector3D::Zero();
//		R = createTransformIJC(ivec, jvec, center);
//	}

//	Transform3D S = createTransformScale(scale);
//	Transform3D T = createTransformTranslate(pos);
//	Transform3D M = T*R*S;
//	return M;
//}

}

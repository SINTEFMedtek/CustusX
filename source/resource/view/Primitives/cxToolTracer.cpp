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


#include "cxToolTracer.h"

#include "vtkImageData.h"
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkProperty.h>
#include <QColor>
#include <vtkMatrix4x4.h>

#include "cxTool.h"
#include "cxBoundingBox3D.h"
#include "cxVolumeHelpers.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"

namespace cx
{

ToolTracerPtr ToolTracer::create(SpaceProviderPtr spaceProvider)
{
	ToolTracerPtr retval(new ToolTracer(spaceProvider));
	return retval;
}

ToolTracer::ToolTracer(SpaceProviderPtr spaceProvider)
{
	mSpaceProvider = spaceProvider;
	mRunning = false;
	mPolyData = vtkPolyDataPtr::New();
	mActor = vtkActorPtr::New();
	mPolyDataMapper = vtkPolyDataMapperPtr::New();

	mPolyDataMapper->SetInputData(mPolyData);
	mActor->SetMapper(mPolyDataMapper);

	mProperty = vtkPropertyPtr::New();
	mActor->SetProperty( mProperty );
	mProperty->SetPointSize(4);

	this->setColor(QColor("red"));

	mPoints = vtkPointsPtr::New();
	mLines = vtkCellArrayPtr::New();

	mPolyData->SetPoints(mPoints);
	mPolyData->SetLines(mLines);
	mPolyData->SetVerts(mLines);
	mFirstPoint = false;
	mMinDistance = -1.0;
	mSkippedPoints = 0;

	mSpaceListener = mSpaceProvider->createListener();
	mSpaceListener->setSpace(CoordinateSystem::patientReference());
	connect(mSpaceListener.get(), &SpaceListener::changed, this, &ToolTracer::onSpaceChanged);
	this->onSpaceChanged();
}

void ToolTracer::start()
{
	if (mRunning)
		return;
	mRunning = true;
	mFirstPoint = true;
	mSkippedPoints = 0;
	this->connectTool();
}

void ToolTracer::stop()
{
	if (!mRunning)
		return;
	this->disconnectTool();
	mRunning = false;
}

void ToolTracer::clear()
{
	mPoints->Reset();
	mLines->Reset();
	mPolyData->Modified();
}

void ToolTracer::connectTool()
{
	if (mTool && mRunning)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		//connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
	}
}

void ToolTracer::disconnectTool()
{
	if (mTool && mRunning)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		//disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
	}
}

void ToolTracer::setColor(QColor color)
{
	mActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
}

void ToolTracer::setTool(ToolPtr tool)
{
	this->disconnectTool();
	mTool = tool;
	this->connectTool();
}

vtkPolyDataPtr ToolTracer::getPolyData()
{
	return mPolyData;
}

vtkActorPtr ToolTracer::getActor()
{
	return mActor;
}

void ToolTracer::onSpaceChanged()
{
	Transform3D rMpr = mSpaceProvider->get_rMpr();
//	std::cout << "rMpr ToolTracer: \n" << rMpr << std::endl;
	mActor->SetUserMatrix(rMpr.getVtkMatrix());
}

bool ToolTracer::isRunning() const
{
	return mRunning;
}

void ToolTracer::receiveTransforms(Transform3D prMt, double timestamp)
{
	Transform3D rMpr = Transform3D::Identity(); // handle rMpr in actor
//	Transform3D rMpr = mSpaceProvider->get_rMpr();
	Transform3D rMt = rMpr * prMt;

	Vector3D p = rMt.coord(Vector3D(0,0,0));

	if (mMinDistance > 0.0)
	{
		if (!mFirstPoint && (mPreviousPoint - p).length() < mMinDistance)
		{
			++mSkippedPoints;
			return;
		}
	}
	mFirstPoint = false;
	mPreviousPoint = p;
	mPoints->InsertNextPoint(p.begin());

	if (mPoints->GetNumberOfPoints() > 1)
	{
		// fill cell points for the entire polydata.
		// This is very ineffective, but I have no idea how to update mLines incrementally.
		mLines->Initialize();
		std::vector<vtkIdType> ids(mPoints->GetNumberOfPoints());
		for (unsigned i=0; i<ids.size(); ++i)
			ids[i] = i;
		mLines->InsertNextCell(ids.size(), &(*ids.begin()));
		
		mPolyData->Modified();
	}
}

void ToolTracer::addManyPositions(TimedTransformMap trackerRecordedData_prMt)
{
    for(TimedTransformMap::iterator iter=trackerRecordedData_prMt.begin(); iter!=trackerRecordedData_prMt.end(); ++iter)
        {
            double timestamp = iter->first;
            Transform3D prMt = iter->second;
            this->receiveTransforms(prMt, timestamp);
        }
}


}

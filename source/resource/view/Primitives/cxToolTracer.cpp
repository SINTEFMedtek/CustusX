/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxLogger.h"

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
	Vector3D p = prMt.coord(Vector3D(0,0,0));

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
		mLines->Modified();
		
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

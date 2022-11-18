/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxReadFbgsMessage.h"
#include <QStringRef>
#include <QStringList>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
//#include <vtkFloatArray.h>
//#include <vtkUnsignedCharArray.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>
#include "cxLogger.h"
#include "cxTransform3D.h"

namespace cx
{
ReadFbgsMessage::ReadFbgsMessage()
{
	//Using code from cxToolTracer as a basis (Used by ToolRep3D)
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

	mAxis.push_back(axisX);
	mAxis.push_back(axisY);
	mAxis.push_back(axisZ);

}

vtkPolyDataPtr ReadFbgsMessage::getPolyData()
{
	return mPolyData;
}

vtkActorPtr ReadFbgsMessage::getActor()
{
	return mActor;
}

void ReadFbgsMessage::set_prMt(Transform3D prMt)
{
	m_prMt = prMt;
	//mActor->SetUserMatrix(prMt.getVtkMatrix());
}

void ReadFbgsMessage::setShapePointLock(int posNumber)
{
	mShapePointLockNumber = posNumber;
}

int ReadFbgsMessage::getRangeMax()
{
	return mRangeMax-1;
}

void ReadFbgsMessage::setColor(QColor color)
{
	mActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
}

QString ReadFbgsMessage::getAxisString(AXIS axis)
{
	switch(axis)
	{
	case axisX:
		return QString("Shape x [cm]");
		break;
	case axisY:
		return QString("Shape y [cm]");
		break;
	case axisZ:
		return QString("Shape z [cm]");
		break;
	case axisCOUNT:
	default:
		return QString();
		break;
	}
}

std::vector<double>* ReadFbgsMessage::getAxisPosVector(AXIS axis)
{
	switch(axis)
	{
	case axisX:
		return &mXaxis;
		break;
	case axisY:
		return &mYaxis;
		break;
	case axisZ:
		return &mZaxis;
		break;
	case axisCOUNT:
	default:
		return nullptr;
		break;
	}
}

void ReadFbgsMessage::readBuffer(QString buffer)
{
	QStringList bufferList = buffer.split("	");
	int pos = 0;
	for(int i = 0; i < mAxis.size(); ++i)
	{
		pos = this->readPosForOneAxis(mAxis[i], bufferList, pos);
		if(pos == -1)
		{
			CX_LOG_WARNING() << "ReadFbgsMessage::readBuffer: Error reading " << getAxisString(mAxis[i]) << " values from TCP socket";
			return;
		}
	}
	this->createPolyData();
}

int ReadFbgsMessage::readPosForOneAxis(AXIS axis, QStringList &bufferList, int previousPos)
{
	int pos = getAxisStringPosition(bufferList, axis, previousPos);
	if(pos == -1)
		return -1;
	else
		++pos;

	int numValues;
	if((bufferList.size() < 2) || !this->toInt(bufferList[pos++], numValues))
		return -1;

	std::vector<double> *axisVextor = getAxisPosVector(axis);
	int stopPos = pos + numValues;
	if(stopPos > bufferList.size())
	{
		CX_LOG_WARNING() << "ReadFbgsMessage::readPosForOneAxis: Buffer don't have enough (" << numValues << ") values along axis: " << getAxisString(axis);
		return -1;
	}
	for(; pos < stopPos; ++pos)
	{
		double value;
		if(!this->toDouble(bufferList[pos], value))
			return -1;
		axisVextor->push_back(value*10);
	}
	return pos;
}

bool ReadFbgsMessage::toInt(QString string, int &value)
{
	bool ok;
	value = string.toInt(&ok);
	if(!ok)
		CX_LOG_WARNING() << "ReadFbgsMessage::toInt: Cannot convert " << string << " to int";
	return ok;
}

bool ReadFbgsMessage::toDouble(QString string, double &value)
{
	string = string.split('\n')[0];
	bool ok;
	value = string.toDouble(&ok);
	if(!ok)
	{
		int intValue;
		ok = this->toInt(string, intValue);
		value = double(intValue);
		if(!ok)
			CX_LOG_WARNING() << "ReadFbgsMessage::toDouble: Cannot convert " << string << " to double";
	}
	return ok;
}

int ReadFbgsMessage::getAxisStringPosition(QStringList &bufferList, AXIS axis, int startFrom)
{
	QString axisString = getAxisString(axis);
	for(int i = startFrom; i < bufferList.size(); ++i)
		if (bufferList[i] == axisString)
			return i;

	CX_LOG_WARNING() << "ReadFbgsMessage::getAxisStringPosition: Could't find separator string: " << axisString;
	return -1;
}

bool ReadFbgsMessage::createPolyData()
{
	this->clearPolyData();
	mRangeMax = mXaxis.size();
	if((mRangeMax != mYaxis.size()) || (mRangeMax != mZaxis.size()))
	{
		CX_LOG_WARNING() << "ReadFbgsMessage::createPolyData: Not equal number of position data in all axes";
		return false;
	}
	for(int i=0; i < mRangeMax; ++i)
	{
		Vector3D p(mXaxis[i], mYaxis[i], mZaxis[i]);
		mPoints->InsertNextPoint(p.begin());
		if(i == mShapePointLockNumber)
			this->lockShape(i);
	}

	// fill cell points for the entire polydata.
	mLines->Initialize();
	std::vector<vtkIdType> ids(mPoints->GetNumberOfPoints());
	for (unsigned i=0; i<ids.size(); ++i)
		ids[i] = i;
	mLines->InsertNextCell(ids.size(), &(*ids.begin()));
	mLines->Modified();

	mPolyData->Modified();
	this->clearAxisVectors();
	return true;
}

Vector3D ReadFbgsMessage::lockShape(int position)
{
	Vector3D p(mXaxis[position], mYaxis[position], mZaxis[position]);

	Transform3D translateToP = createTransformTranslate(p);
	Vector3D delta_p = getDeltaPosition(position);
	Transform3D rotatePdirectionToZaxis = createTransformRotationBetweenVectors(delta_p, Vector3D::UnitZ());

	Transform3D prMshape = m_prMt * rotatePdirectionToZaxis * translateToP.inv();
	mActor->SetUserMatrix(prMshape.getVtkMatrix());

	return p;
}

Vector3D ReadFbgsMessage::getDeltaPosition(int pos)
{
	int localRange = 5;
	if(mRangeMax < 2)
		return Vector3D(0,0,1);
	else if(mRangeMax < localRange * 2)
		localRange = mRangeMax/2;

	int pos1 = pos - localRange;
	int pos2 = pos + localRange;
	int shift = 0;
	if(pos1 < 0)
		shift = pos1;
	if(pos2 >= mRangeMax)
		shift = pos2 - mRangeMax + 1;
	pos1 -= shift;
	pos2 -= shift;
	Vector3D p1 = Vector3D(mXaxis[pos1], mYaxis[pos1], mZaxis[pos1]);
	Vector3D p2 = Vector3D(mXaxis[pos2], mYaxis[pos2], mZaxis[pos2]);
	Vector3D delta_p = p2-p1;
	return delta_p;
}

void ReadFbgsMessage::clearPolyData()
{
	mPoints->Reset();
	mLines->Reset();
	mPolyData->Modified();
}

void ReadFbgsMessage::clearAxisVectors()
{
	mXaxis.clear();
	mYaxis.clear();
	mZaxis.clear();
}
}//cx

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
	mActor->SetUserMatrix(prMt.getVtkMatrix());
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
		//if(!this->readShape(mAxis[i], buffer))
		if(pos == -1)
		{
			CX_LOG_WARNING() << "Error reading " << getAxisString(mAxis[i]) << " values from TCP socket";
			//return;
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
		pos++;

	int numValues;
	if(!this->toInt(bufferList[pos++], numValues))
		return -1;

	std::vector<double> axisVextor = *getAxisPosVector(axis);
	for(; pos < (pos + numValues); ++pos)
	{
		double value;
		if(!this->toDouble(bufferList[pos], value))
			return -1;
		axisVextor.push_back(value);
	}
	return pos;
}

bool ReadFbgsMessage::toInt(QString string, int &value)
{
	bool ok;
	value = string.toInt(&ok);
	if(!ok)
		CX_LOG_DEBUG() << "Cannot convert " << string << " to int";
	return ok;
}

bool ReadFbgsMessage::toDouble(QString string, double &value)
{
	bool ok;
	value = string.toDouble(&ok);
	if(!ok)
	{
		int intValue;
		ok = this->toInt(string, intValue);
		value = double(intValue);
		if(!ok)
			CX_LOG_DEBUG() << "Cannot convert " << string << " to double";
	}
	return ok;
}

int ReadFbgsMessage::getAxisStringPosition(QStringList &bufferList, AXIS axis, int startFrom)
{
	QString axisString = getAxisString(axis);
	for(int i = startFrom; i < bufferList.size(); ++i)
		if (bufferList[i] == axisString)
			return i;

	CX_LOG_DEBUG() << "could't find separator string: " << axisString;
	return -1;
}

//bool ReadFbgsMessage::readShape(AXIS axis, QString buffer)
//{
//	QString axisString = getAxisString(axis);
//	int bufferPos = 0;
//	bufferPos = buffer.indexOf(axisString, bufferPos, Qt::CaseInsensitive);
//	if(bufferPos == -1)
//	{
//		CX_LOG_WARNING() << "Cannot read " << axisString << " from TCP socket";
//		return false;
//	}
//	bufferPos += axisString.size();
//	return this->readPositions(axis, buffer, bufferPos);
//}

//bool ReadFbgsMessage::readPositions(AXIS axis, QString buffer, int bufferPos)
//{
//	bool ok;
//	//QStringRef numberString(&buffer, bufferPos, sizeof(int));
//	QString numberString = buffer.mid(bufferPos, sizeof(int));
//	int numValues = numberString.toInt(&ok);
//	if(!ok)
//	{
//		CX_LOG_DEBUG() << "Cannot convert " << numberString << " to int";
//		return false;
//	}
//	else
//		CX_LOG_DEBUG() << "Read " << numValues << " positions";//Test debug
//	bufferPos += sizeof(int);

//	std::vector<double> axisVextor = *getAxisPosVector(axis);

//	//QStringRef numbers = QStringRef(&buffer, bufferPos, buffer.size() - bufferPos);
//	QString numbers = buffer.mid(bufferPos);
//	//QStringList numberList = numbers.split("	");

//	for(int i = 0; i < numValues; ++i)
//	{
//		//Use sizeof(float) instead?
//		//numberString = QStringRef(&buffer, bufferPos, sizeof(double));
//		//double value = numberString.toDouble(&ok);

//		//double value = numberList[i].toDouble(&ok);
//		//Alternative without creating QStringList
//		double value = numbers.section("	", i, i).toDouble(&ok);

//		axisVextor.push_back(value);
//		if(!ok)
//		{
//			//CX_LOG_DEBUG() << "Cannot convert number " << i << " value: " << numberList[i] << " to double";
//			CX_LOG_DEBUG() << "Cannot convert number " << i << " value: " << numbers.section("	", i, i) << " to double";
//			return false;
//		}
//		//bufferPos += sizeof(double);
//	}
//	return true;
//}

bool ReadFbgsMessage::createPolyData()
{
	this->clearPolyData();
	if((mXaxis.size() != mYaxis.size()) || (mXaxis.size() != mZaxis.size()))
	{
		CX_LOG_WARNING() << "Not equal number of position data in all axes";
		return false;
	}
	for(int i=0; i < mXaxis.size(); ++i)
	{
		Vector3D p(mXaxis[i], mYaxis[i], mZaxis[i]);
		mPoints->InsertNextPoint(p.begin());
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

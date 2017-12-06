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

#include "cxProbeDefinitionFromStringMessages.h"

#include <vtkXMLDataElement.h>
#include <vtkXMLUtilities.h>
#include <vtkImageData.h>

#include "cxLogger.h"

namespace cx
{

ProbeDefinitionFromStringMessages::ProbeDefinitionFromStringMessages() :
	mSectorInfo(new SectorInfo)
{}

void ProbeDefinitionFromStringMessages::reset()
{
	mSectorInfo->reset();
}

void ProbeDefinitionFromStringMessages::parseStringMessage(igtlio::BaseConverter::HeaderData header, QString message)
{
	//Don't check equipmentType for now
//	if(header.equipmentType != US_PROBE && header.equipmentType != TRACKED_US_PROBE)
//		return;

//	CX_LOG_DEBUG() << "header.equipmentId: " << header.equipmentId;
//	CX_LOG_DEBUG() << "header.deviceName: " << header.deviceName;
//	CX_LOG_DEBUG() << "message: " << message;

	//Test: Don't use XML for now
	QString name = QString(header.deviceName.c_str());
	QString value = message;
	this->parseValue(name, value);
}

std::vector<double> ProbeDefinitionFromStringMessages::toDoubleVector(QString values, QString separator)
{
	std::vector<double> retval;
	QStringList valueList = values.split(separator);
	for (int i = 0; i < valueList.size(); ++i)
	{
		double doublevalue = valueList[i].toDouble();
		retval.push_back(doublevalue);
	}
	return retval;
}

void ProbeDefinitionFromStringMessages::parseValue(QString name, QString value)
{
	int intValue = value.toInt();
	double doubleValue = value.toDouble();
	std::vector<double> doubleVector = toDoubleVector(value);

//	CX_LOG_DEBUG() << "parseStringMessage: "	<< " name: " << name
//				   << " intValue: " << intValue
//				   << " doubleValue: " << doubleValue;

	if (name == "ProbeType")
	{
		if (mSectorInfo->mProbeType != intValue)
		{
			mSectorInfo->mProbeType = intValue;
		}
	}
	//New standard
	else if (name == "Origin")
	{
		if(mSectorInfo->mOrigin != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mOrigin = doubleVector;
		}
	}
	else if (name == "Angles")
	{
		if(mSectorInfo->mAngles != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mAngles = doubleVector;
		}
	}
	else if (name == "BouningBox")
	{
		if(mSectorInfo->mBouningBox != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mBouningBox = doubleVector;
		}
	}
	else if (name == "Depths")
	{
		if(mSectorInfo->mDepths != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mDepths = doubleVector;
		}
	}
	else if (name == "LinearWidth")
	{
		if(mSectorInfo->mLinearWidth != doubleValue)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mLinearWidth = doubleValue;
		}
	}
	else if (name == "SpacingX")
	{
		mSectorInfo->mSpacingX = doubleValue;
	}
	else if (name == "SpacingY")
	{
		mSectorInfo->mSpacingY = doubleValue;
	}
}

void ProbeDefinitionFromStringMessages::setImage(ImagePtr image)
{
	mSectorInfo->mImage = image;
}

bool ProbeDefinitionFromStringMessages::haveValidValues()
{
	return mSectorInfo->isValid();
}

bool ProbeDefinitionFromStringMessages::haveChanged()
{
	return mSectorInfo->haveChanged();
}

ProbeDefinitionPtr ProbeDefinitionFromStringMessages::createProbeDefintion(QString uid)
{
	mSectorInfo->mHaveChanged = false;

	if(!this->haveValidValues())
		return ProbeDefinitionPtr();

	//Send spacing as messages for now. Should be sent together with image.
	mSectorInfo->mImage->getBaseVtkImageData()->SetSpacing(mSectorInfo->mSpacingX, mSectorInfo->mSpacingY, 1.0);
	Vector3D spacing = mSectorInfo->mImage->getSpacing();
	Vector3D origin_p(mSectorInfo->mOrigin[0], mSectorInfo->mOrigin[1], mSectorInfo->mOrigin[2]);

	ProbeDefinitionPtr probeDefinition = this->initProbeDefinition();
	probeDefinition->setUid(uid);
	probeDefinition->setOrigin_p(origin_p);
	probeDefinition->setSpacing(spacing);
	probeDefinition->setClipRect_p(this->getBoundinBox());
	probeDefinition->setSector(mSectorInfo->mDepths[0], mSectorInfo->mDepths[1], this->getWidth());
	probeDefinition->setSize(this->getSize());
	probeDefinition->setUseDigitalVideo(true);

	return probeDefinition;
}

ProbeDefinitionPtr ProbeDefinitionFromStringMessages::initProbeDefinition()
{
	ProbeDefinitionPtr probeDefinition;
	if(mSectorInfo->mProbeType == 2) //linear
	{
		probeDefinition = ProbeDefinitionPtr(new ProbeDefinition(ProbeDefinition::tLINEAR));
	}
	else if (mSectorInfo->mProbeType == 1)//sector
	{
		probeDefinition = ProbeDefinitionPtr(new ProbeDefinition(ProbeDefinition::tSECTOR));
	}
	else
	{
		CX_LOG_ERROR() << "ProbeDefinitionFromStringMessages::initProbeDefinition: Incorrect probe type: " << mSectorInfo->mProbeType;
	}
	return probeDefinition;
}

double ProbeDefinitionFromStringMessages::getWidth()
{
	double width = 0;
	if(mSectorInfo->mProbeType == 2) //linear
	{
		width = mSectorInfo->mLinearWidth;
	}
	else if (mSectorInfo->mProbeType == 1)//sector
	{
		width = mSectorInfo->mAngles[1] - mSectorInfo->mAngles[0];
	}
	return width;
}

QSize ProbeDefinitionFromStringMessages::getSize()
{
	Eigen::Array3i dimensions(mSectorInfo->mImage->getBaseVtkImageData()->GetDimensions());
	QSize size(dimensions[0], dimensions[1]);
	return size;
}

DoubleBoundingBox3D ProbeDefinitionFromStringMessages::getBoundinBox()
{
	double zStart = 0;
	double zEnd = 0;
	if(mSectorInfo->mBouningBox.size() == 6)
	{
		zStart = mSectorInfo->mBouningBox[4];
		zEnd = mSectorInfo->mBouningBox[5];
	}
	DoubleBoundingBox3D retval(mSectorInfo->mBouningBox[0], mSectorInfo->mBouningBox[1],
			mSectorInfo->mBouningBox[2], mSectorInfo->mBouningBox[3],
			zStart, zEnd);
	return retval;
}

}//cx

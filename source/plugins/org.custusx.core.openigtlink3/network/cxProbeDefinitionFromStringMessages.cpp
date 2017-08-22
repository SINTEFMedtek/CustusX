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
	mTestMode(false),
	mSectorInfo(new SectorInfo)
{}

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
			if(mTestMode)
			{
				// 0 = unknown, 1 = sector, 2 = linear
				mSectorInfo->mProbeType = 1;
				CX_LOG_WARNING() << "ProbeDefinitionFromStringMessages only using dummy values";
			}
		}
	}
	//New standard
	else if (name == "Origin")
	{
		if(mSectorInfo->mOrigin != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mOrigin = doubleVector;
			mSectorInfo->mNewStandard = true;
		}
	}
	else if (name == "Angles")
	{
		if(mSectorInfo->mAngles != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mAngles = doubleVector;
			mSectorInfo->mNewStandard = true;
		}
	}
	else if (name == "BouningBox")
	{
		if(mSectorInfo->mBouningBox != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mBouningBox = doubleVector;
			mSectorInfo->mNewStandard = true;
		}
	}
	else if (name == "Depths")
	{
		if(mSectorInfo->mDepths != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mDepths = doubleVector;
			mSectorInfo->mNewStandard = true;
		}
	}
	else if (name == "LinearWidth")
	{
		if(mSectorInfo->mLinearWidth != doubleValue)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mLinearWidth = doubleValue;
			mSectorInfo->mNewStandard = true;
		}
	}

	//Old values - to be removed

	else if (name == "StartDepth")
	{
		mSectorInfo->mStartDepth = doubleValue;
		if(mTestMode)
			mSectorInfo->mStartDepth = 10;
	}
	else if (name == "StopDepth")
	{
		mSectorInfo->mStopDepth = doubleValue;
		if(mTestMode)
			mSectorInfo->mStopDepth = 110;
	}
	else if (name == "StartLineX")
	{
		mSectorInfo->mStartLineX = doubleValue;
		if(mTestMode)
			mSectorInfo->mStartLineX = 30;
	}
	else if (name == "StartLineY")
	{
		mSectorInfo->mStartLineY = doubleValue;
		if(mTestMode)
			mSectorInfo->mStartLineY = 30;
	}
	else if (name == "StopLineX")
	{
		mSectorInfo->mStopLineX = doubleValue;

		//Turn on testmode
		if(similar(doubleValue,0))
		{
			mSectorInfo->reset();
			mTestMode = true;
		}

		if(mTestMode)
			mSectorInfo->mStopLineX = 300;
	}
	else if (name == "StopLineY")
	{
		mSectorInfo->mStopLineY = doubleValue;
		if(mTestMode)
			mSectorInfo->mStopLineY = 30;
	}
	else if (name == "StartLineAngle")
	{
		mSectorInfo->mStartLineAngle = doubleValue;
		if(mTestMode)
			mSectorInfo->mStartLineAngle = 1.0;
	}
	else if (name == "StopLineAngle")
	{
		mSectorInfo->mStopLineAngle = doubleValue;
		if(mTestMode)
			mSectorInfo->mStopLineAngle = 1.5;
	}
	else if (name == "SpacingX")
	{
		mSectorInfo->mSpacingX = doubleValue;
		if(mTestMode)
			mSectorInfo->mSpacingX = 1.0;
	}
	else if (name == "SpacingY")
	{
		mSectorInfo->mSpacingY = doubleValue;
		if(mTestMode)
			mSectorInfo->mSpacingY = 1.0;
	}
	else if (name == "SectorLeftPixels")
	{
		mSectorInfo->mSectorLeftPixels = intValue;
		if(mTestMode)
			mSectorInfo->mSectorLeftPixels = 10;
	}
	else if (name == "SectorRightPixels")
	{
		mSectorInfo->mSectorRightPixels = intValue;
		if(mTestMode)
			mSectorInfo->mSectorRightPixels = 500;
	}
	else if (name == "SectorTopPixels")
	{
		mSectorInfo->mSectorTopPixels = intValue;
		if(mTestMode)
			mSectorInfo->mSectorTopPixels = 10;
	}
	else if (name == "SectorBottomPixels")
	{
		mSectorInfo->mSectorBottomPixels = intValue;
		if(mTestMode)
			mSectorInfo->mSectorBottomPixels = 300;
	}
	else if (name == "SectorLeftMm")
	{
		mSectorInfo->mSectorLeftMm = doubleValue;
		if(mTestMode)
			mSectorInfo->mSectorLeftMm = -10.0;
	}
	else if (name == "SectorRightMm")
	{
		mSectorInfo->mSectorRightMm = doubleValue;
		if(mTestMode)
			mSectorInfo->mSectorRightMm = 10.0;
	}
	else if (name == "SectorTopMm")
	{
		mSectorInfo->mSectorTopMm = doubleValue;
		if(mTestMode)
			mSectorInfo->mSectorTopMm = 25.0;
	}
	else if (name == "SectorBottomMm")
	{
		mSectorInfo->mSectorBottomMm = doubleValue;
		if(mTestMode)
			mSectorInfo->mSectorBottomMm = 0.0;
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

ProbeDefinitionPtr ProbeDefinitionFromStringMessages::createProbeDefintionFromStandardValues(QString uid)
{
	mSectorInfo->mHaveChanged = false;

	if(!mSectorInfo->standardIsValid())
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

ProbeDefinitionPtr ProbeDefinitionFromStringMessages::createProbeDefintion(QString uid)
{
	if(!this->haveValidValues())
		return ProbeDefinitionPtr();

	if(mSectorInfo->mNewStandard)
		return this->createProbeDefintionFromStandardValues(uid);


	Vector3D spacing = mSectorInfo->mImage->getSpacing();
//	CX_LOG_DEBUG() << "Original spacing: " << spacing;
	DoubleBoundingBox3D boundingBox = mSectorInfo->mImage->boundingBox();

	//The mm values received from BK is not in picture coords, but relative to an origin between startLine and stopLine
	//Need values from B_GEOMETRY_PIXEL and B_GEOMETRY_TISSUE to complete calculation.

//	CX_LOG_DEBUG() << "boundingBox: " << boundingBox;
//	CX_LOG_DEBUG() << "mProbeType: " << mSectorInfo->mProbeType;
//	CX_LOG_DEBUG() << "mStartDepth: " << mSectorInfo->mStartDepth << " mStopDepth: " << mSectorInfo->mStopDepth;
//	CX_LOG_DEBUG() << "mStartLineX: " << mSectorInfo->mStartLineX << " mStartLineY: " << mSectorInfo->mStartLineY;
//	CX_LOG_DEBUG() << "mStopLineX: " << mSectorInfo->mStopLineX << " mStopLineY: " << mSectorInfo->mStopLineY;
//	CX_LOG_DEBUG() << "mStartLineAngle: " << mSectorInfo->mStartLineAngle << " mStopLineAngle: " << mSectorInfo->mStopLineAngle;
//	CX_LOG_DEBUG() << "mSpacingX: " << mSectorInfo->mSpacingX << " mSpacingY: " << mSectorInfo->mSpacingY;
	//These values gives the scanner screen coordinates that the transmitted image is sent from
	// Image dims: 668 544
	// left 125, right 792, diff: 667
	// top 262, bottom 805, diff: 543
//	CX_LOG_DEBUG() << "mSectorLeftPixels: " << mSectorInfo->mSectorLeftPixels << " mSectorRightPixels: " << mSectorInfo->mSectorRightPixels;
//	CX_LOG_DEBUG() << "mSectorTopPixels: " << mSectorInfo->mSectorTopPixels << " mSectorBottomPixels: " << mSectorInfo->mSectorBottomPixels;

	mSectorInfo->mImage->getBaseVtkImageData()->SetSpacing(mSectorInfo->mSpacingX, mSectorInfo->mSpacingY, 1.0);
	spacing = mSectorInfo->mImage->getSpacing();
//	CX_LOG_DEBUG() << "New spacing: " << spacing;
	Eigen::Array3i dimensions(mSectorInfo->mImage->getBaseVtkImageData()->GetDimensions());
//	CX_LOG_DEBUG() << "Image size: " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2];

	double width = 0;
	Vector3D origin_p(0, 0, 0);

//	int centerX_pix = (mSectorInfo->mSectorRightPixels-mSectorInfo->mSectorLeftPixels)/2 + mSectorInfo->mSectorLeftPixels;
	int centerX_pix = dimensions[0]/2;
	origin_p[0] = centerX_pix;

	double originHeightAboveBox_mm = 0.0;
	double originDistanceToStartLine_mm = 0.0;

	ProbeDefinitionPtr probeDefinition;
	if(mSectorInfo->mProbeType == 2) //linear
	{
		probeDefinition = ProbeDefinitionPtr(new ProbeDefinition(ProbeDefinition::tLINEAR));
		width = fabs(mSectorInfo->mStartLineX - mSectorInfo->mStopLineX);
//		origin_p[1] = mSectorInfo->mSectorTopPixels;
		origin_p[1] = 0;
//		CX_LOG_DEBUG() << "width: " << width;
	}
	else if (mSectorInfo->mProbeType == 1)//sector
	{
		probeDefinition = ProbeDefinitionPtr(new ProbeDefinition(ProbeDefinition::tSECTOR));
		width = fabs(mSectorInfo->mStartLineAngle - mSectorInfo->mStopLineAngle);

		originHeightAboveBox_mm = (mSectorInfo->mStartLineX) / tan(width / 2.0);
		originDistanceToStartLine_mm = (mSectorInfo->mStartLineX) / sin(width / 2.0);
//		origin_p[1] = mSectorInfo->mSectorTopPixels - ( originHeightAboveBox_mm / spacing[1]);
		origin_p[1] = 0 - ( originHeightAboveBox_mm / spacing[1]);//Should be correct?
//		CX_LOG_DEBUG() << "origin_p[1]: " << origin_p[1] << " originHeightAboveBox_mm: " << originHeightAboveBox_mm << " originDistanceToStartLine_mm: " << originDistanceToStartLine_mm;
	}
	else
	{
		CX_LOG_ERROR() << "ProbeDefinitionFromStringMessages::createProbeDefintion: Incorrect probe type: " << mSectorInfo->mProbeType;
	}

	double depthStart = mSectorInfo->mStartDepth + originDistanceToStartLine_mm;
	double depthEnd = mSectorInfo->mStopDepth + originDistanceToStartLine_mm;
	QSize size(dimensions[0], dimensions[1]);

	probeDefinition->setUid(uid);
	probeDefinition->setOrigin_p(origin_p);
	probeDefinition->setSpacing(spacing);
	//DoubleBoundingBox3D clipRect(mSectorInfo->mSectorLeftPixels, mSectorInfo->mSectorRightPixels,
	//							 mSectorInfo->mSectorTopPixels, mSectorInfo->mSectorBottomPixels);
	DoubleBoundingBox3D clipRect(0, dimensions[0]-1,
								 0, dimensions[1]-1);
	probeDefinition->setClipRect_p(clipRect);
	probeDefinition->setSector(depthStart, depthEnd, width);
	probeDefinition->setSize(size);
	probeDefinition->setUseDigitalVideo(true);

	mSectorInfo->reset();
	mTestMode = false;

	return probeDefinition;
}

}//cx
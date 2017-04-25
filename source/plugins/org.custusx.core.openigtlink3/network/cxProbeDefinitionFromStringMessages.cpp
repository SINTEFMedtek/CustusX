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
	mTestMode(false)
{}

void ProbeDefinitionFromStringMessages::parseStringMessage(igtlio::BaseConverter::HeaderData header, QString message)
{
	//Don't check equipmentType for now
//	if(header.equipmentType != US_PROBE && header.equipmentType != TRACKED_US_PROBE)
//		return;
	CX_LOG_DEBUG() << "header.equipmentId: " << header.equipmentId;
	CX_LOG_DEBUG() << "header.deviceName: " << header.deviceName;
	CX_LOG_DEBUG() << "message: " << message;

	//Test: Don't use XML for now
	QString name = QString(header.deviceName.c_str());
	QString value = message;
	int intValue = value.toInt();
	double doubleValue = value.toDouble();
	CX_LOG_DEBUG() << "parseStringMessage: "	<< " name: " << name
//				   << " value: " << value
				   << " intValue: " << intValue
				   << " doubleValue: " << doubleValue;

	if (name == "ProbeType")
	{
		mSectorInfo.mProbeType = intValue;
		if(mTestMode)
		{
			// 0 = sector, 1 = linear
//			mSectorInfo.mProbeType = 0;
			CX_LOG_WARNING() << "ProbeDefinitionFromStringMessages only using dummy values";
		}
	}
	else if (name == "StartDepth")
	{
		mSectorInfo.mStartDepth = doubleValue;
		if(mTestMode)
			mSectorInfo.mStartDepth = 10;
	}
	else if (name == "StopDepth")
	{
		mSectorInfo.mStopDepth = doubleValue;
		if(mTestMode)
			mSectorInfo.mStopDepth = 110;
	}
	else if (name == "StartLineX")
	{
		mSectorInfo.mStartLineX = doubleValue;
		if(mTestMode)
			mSectorInfo.mStartLineX = 30;
	}
	else if (name == "StartLineY")
	{
		mSectorInfo.mStartLineY = doubleValue;
		if(mTestMode)
			mSectorInfo.mStartLineY = 30;
	}
	else if (name == "StopLineX")
	{
		mSectorInfo.mStopLineX = doubleValue;

		//Turn on testmode
		if(similar(doubleValue,0))
		{
			mTestMode = true;
		}

		if(mTestMode)
			mSectorInfo.mStopLineX = 300;
	}
	else if (name == "StopLineY")
	{
		mSectorInfo.mStopLineY = doubleValue;
		if(mTestMode)
			mSectorInfo.mStopLineY = 30;
	}
	else if (name == "StartLineAngle")
	{
		mSectorInfo.mStartLineAngle = doubleValue;
		if(mTestMode)
			mSectorInfo.mStartLineAngle = 1.0;
	}
	else if (name == "StopLineAngle")
	{
		mSectorInfo.mStopLineAngle = doubleValue;
		if(mTestMode)
			mSectorInfo.mStopLineAngle = 0.5;
	}
}

void ProbeDefinitionFromStringMessages::setImage(ImagePtr image)
{
	mSectorInfo.mImage = image;
}

bool ProbeDefinitionFromStringMessages::haveValidValues()
{
	return mSectorInfo.isValid();
}

ProbeDefinitionPtr ProbeDefinitionFromStringMessages::createProbeDefintion(QString uid)
{
	if(!this->haveValidValues())
		return ProbeDefinitionPtr();


	Vector3D spacing = mSectorInfo.mImage->getSpacing();
	DoubleBoundingBox3D boundingBox = mSectorInfo.mImage->boundingBox();

	CX_LOG_DEBUG() << "boundingBox: " << boundingBox;
	CX_LOG_DEBUG() << "spacing: " << spacing;
	CX_LOG_DEBUG() << "mProbeType: " << mSectorInfo.mProbeType;
	CX_LOG_DEBUG() << "mStartDepth: " << mSectorInfo.mStartDepth << " mStopDepth: " << mSectorInfo.mStopDepth;
	CX_LOG_DEBUG() << "mStartLineX: " << mSectorInfo.mStartLineX << " mStartLineY: " << mSectorInfo.mStartLineY;
	CX_LOG_DEBUG() << "mStopLineX: " << mSectorInfo.mStopLineX << " mStopLineY: " << mSectorInfo.mStopLineY;
	CX_LOG_DEBUG() << "mStartLineAngle: " << mSectorInfo.mStartLineAngle << " mStopLineAngle: " << mSectorInfo.mStopLineAngle;

	double width = 0;
	Vector3D origin_p(0, 0, 0);

	int centerX_pix = fabs(mSectorInfo.mStartLineX - mSectorInfo.mStopLineX) / 2 + mSectorInfo.mStartLineX;
	origin_p[0] = centerX_pix;


	ProbeDefinitionPtr probeDefinition;
	if(mSectorInfo.mProbeType == 1) //linear
	{
		probeDefinition = ProbeDefinitionPtr(new ProbeDefinition(ProbeDefinition::tLINEAR));
		width = fabs(mSectorInfo.mStartLineX - mSectorInfo.mStopLineX);
		origin_p[1] = mSectorInfo.mStartLineY * spacing[1];
	}
	else if (mSectorInfo.mProbeType == 0)//sector
	{
		probeDefinition = ProbeDefinitionPtr(new ProbeDefinition(ProbeDefinition::tSECTOR));
		width = fabs(mSectorInfo.mStartLineAngle - mSectorInfo.mStopLineAngle);
		origin_p[1] = tan(width / 2.0) * (centerX_pix - mSectorInfo.mStartLineX) * spacing[1];
	}
	else
	{
		CX_LOG_ERROR() << "ProbeDefinitionFromStringMessages::createProbeDefintion: Incorrect probe type: " << mSectorInfo.mProbeType;
	}

	double depthStart = mSectorInfo.mStartDepth + origin_p[1];
	double depthEnd = mSectorInfo.mStopDepth + origin_p[1];
	Eigen::Array3i dimensions(mSectorInfo.mImage->getBaseVtkImageData()->GetDimensions());
	QSize size(dimensions[0], dimensions[1]);

	probeDefinition->setUid(uid);
	probeDefinition->setOrigin_p(origin_p);
	probeDefinition->setSpacing(spacing);
	probeDefinition->setClipRect_p(boundingBox);
	probeDefinition->setSector(depthStart, depthEnd, width);
	probeDefinition->setSize(size);
	probeDefinition->setUseDigitalVideo(true);

	return probeDefinition;
}

}//cx
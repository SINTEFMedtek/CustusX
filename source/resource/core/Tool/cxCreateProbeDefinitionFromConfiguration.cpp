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

#include "cxCreateProbeDefinitionFromConfiguration.h"
#include "cxTypeConversions.h"
#include <iostream>

namespace cx
{

ProbeXmlConfigParser::Configuration createConfigurationFromProbeDefinition(ProbeXmlConfigParser::Configuration basis, ProbeDefinition data)
{
	ProbeXmlConfigParser::Configuration config = basis;

	QSize storedSize(basis.mImageWidth, basis.mImageHeight);
	if (storedSize!=data.getSize())
	{
		// wrong size: resample
		data.resample(storedSize);
	}

	config.mLeftEdge =  data.getClipRect_p()[0];
	config.mRightEdge =  data.getClipRect_p()[1];
	config.mTopEdge =  data.getClipRect_p()[2];
	config.mBottomEdge =  data.getClipRect_p()[3];

	config.mOriginCol = data.getOrigin_p()[0];
	config.mOriginRow = data.getOrigin_p()[1];

	config.mPixelWidth = data.getSpacing()[0];
	config.mPixelHeight = data.getSpacing()[1];

	config.mImageWidth = data.getSize().width();
	config.mImageHeight = data.getSize().height();

	if (data.getType()==ProbeDefinition::tSECTOR)
	{
		config.mWidthDeg = data.getWidth() / M_PI*180.0;
		config.mOffset = data.getDepthStart() / data.getSpacing()[1];
		config.mDepth = (data.getDepthEnd() - data.getDepthStart()) / data.getSpacing()[1];
	}
	else
	{
		// derived values
		config.mWidthDeg = 0.0;
		config.mOffset = 0;
		config.mDepth = 0;
	}

	config.mTemporalCalibration = data.getTemporalCalibration();

	return config;
}

ProbeDefinition createProbeDefinitionFromConfiguration(ProbeXmlConfigParser::Configuration config)
{
  if(config.isEmpty())
    return ProbeDefinition();

    ProbeDefinition probeDefinition;

  if (config.mWidthDeg > 0.1) // Sector probe
  {
	double depthStart = config.mOffset * config.mPixelHeight;
	double depthEnd = config.mDepth * config.mPixelHeight + depthStart;

	double width = config.mWidthDeg * M_PI / 180.0;//width in radians
	probeDefinition = ProbeDefinition(ProbeDefinition::tSECTOR);
	probeDefinition.setSector(depthStart, depthEnd, width);
  }
  else //Linear probe
  {
    int widtInPixels = config.mRightEdge - config.mLeftEdge;
    double width = config.mPixelWidth * double(widtInPixels); //width in mm
    // correct for top/bottom edges if applicable
    double depthStart = double(config.mTopEdge-config.mOriginRow) * config.mPixelHeight;
    double depthEnd = double(config.mBottomEdge-config.mOriginRow) * config.mPixelHeight;

	probeDefinition = ProbeDefinition(ProbeDefinition::tLINEAR);
	probeDefinition.setSector(depthStart, depthEnd, width);
  }

	probeDefinition.setSpacing(Vector3D(config.mPixelWidth, config.mPixelHeight, 1));
	probeDefinition.setSize(QSize(config.mImageWidth, config.mImageHeight));
	probeDefinition.setOrigin_p(Vector3D(config.mOriginCol, config.mOriginRow, 0));
	probeDefinition.setClipRect_p(DoubleBoundingBox3D(config.mLeftEdge,config.mRightEdge,config.mTopEdge,config.mBottomEdge,0,0));
	probeDefinition.setTemporalCalibration(config.mTemporalCalibration);

	return probeDefinition;
}

} // namespace cx

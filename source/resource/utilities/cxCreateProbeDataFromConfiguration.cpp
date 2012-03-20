/*
 * cxCreateProbeDataFromConfiguration.cpp
 *
 *  \date Nov 26, 2010
 *      \author christiana
 */

#include "cxCreateProbeDataFromConfiguration.h"
#include <iostream>

ProbeXmlConfigParser::Configuration createConfigurationFromProbeData(ProbeXmlConfigParser::Configuration basis, ssc::ProbeData data)
{
	ProbeXmlConfigParser::Configuration config = basis;
//	config.mConfigId = uid;
//	config.mName = name;

	config.mLeftEdge = data.mImage.mClipRect_p[0];
	config.mRightEdge = data.mImage.mClipRect_p[1];
	config.mTopEdge = data.mImage.mClipRect_p[2];
	config.mBottomEdge = data.mImage.mClipRect_p[3];

	config.mOriginCol = data.mImage.mOrigin_p[0];
	config.mOriginRow = data.mImage.mOrigin_p[1];

	config.mPixelWidth = data.mImage.mSpacing[0];
	config.mPixelHeight = data.mImage.mSpacing[1];

	config.mImageWidth = data.mImage.mSize.width();
	config.mImageHeight = data.mImage.mSize.height();

	if (data.mType==ssc::ProbeData::tSECTOR)
	{
		config.mWidthDeg = data.mWidth / M_PI*180.0;
		config.mOffset = data.mDepthStart / data.mImage.mSpacing[1];
		config.mDepth = (data.mDepthEnd - data.mDepthStart) / data.mImage.mSpacing[1];
	}
	else
	{
		// derived values
		config.mWidthDeg = 0.0;
		config.mOffset = 0;
		config.mDepth = 0;
	}

	config.mTemporalCalibration = data.mTemporalCalibration;

	return config;
}

ssc::ProbeData createProbeDataFromConfiguration(ProbeXmlConfigParser::Configuration config)
{
//	std::cout << "createProbeDataFromConfiguration()" << std::endl;
  if(config.isEmpty())
    return ssc::ProbeData();

  double depthStart = config.mOffset * config.mPixelHeight;
  double depthEnd = config.mDepth * config.mPixelHeight + depthStart;

  ssc::ProbeData::ProbeImageData imageData;
  imageData.mSpacing = ssc::Vector3D(config.mPixelWidth, config.mPixelHeight, 1);
  imageData.mSize = QSize(config.mImageWidth, config.mImageHeight);
  imageData.mOrigin_p = ssc::Vector3D(config.mOriginCol, config.mOriginRow, 0);
  imageData.mClipRect_p = ssc::DoubleBoundingBox3D(config.mLeftEdge,config.mRightEdge,config.mTopEdge,config.mBottomEdge,0,0);

  // find the origin in a mm-based, lower-left-corner coord space:
//  ssc::Vector3D c(config.mOriginCol, config.mImageHeight - config.mOriginRow - 1, 0);
//  c = multiply_elems(c, imageData.mSpacing);
////  c[1] -= depthStart; // config def of origin is before offset, probesector def is after offset (physical probe tip)
//  imageData.mOrigin_u = c;

  ssc::ProbeData probeSector;
  if (config.mWidthDeg > 0.1) // Sector probe
  {
    double width = config.mWidthDeg * M_PI / 180.0;//width in radians
    probeSector = ssc::ProbeData(ssc::ProbeData::tSECTOR, depthStart, depthEnd, width);
  }
  else //Linear probe
  {
    int widtInPixels = config.mRightEdge - config.mLeftEdge;
    double width = config.mPixelWidth * widtInPixels; //width in mm
    // correct for top/bottom edges if applicable
    depthStart = std::max(depthStart, (config.mTopEdge-config.mOriginRow) * config.mPixelHeight);
    depthEnd = std::min(depthEnd, (config.mBottomEdge-config.mOriginRow) * config.mPixelHeight);

    probeSector = ssc::ProbeData(ssc::ProbeData::tLINEAR, depthStart, depthEnd, width);
  }

  probeSector.mImage = imageData;
  probeSector.mTemporalCalibration = config.mTemporalCalibration;

//  std::cout << "DepthStart" << probeSector.mDepthStart << std::endl;
//  std::cout << "DepthEnd" << probeSector.mDepthEnd << std::endl;
//  std::cout << "Type" << probeSector.mType << std::endl;
//  std::cout << "Width" << probeSector.mWidth << std::endl;
//  std::cout << "Origin_u" << probeSector.mImage.mOrigin_u << std::endl;
//  std::cout << "Spacing" << probeSector.mImage.mSpacing << std::endl;
//  std::cout << "Image size" << probeSector.mImage.mSize.width() << "," << probeSector.mImage.mSize.height() << std::endl;

  return probeSector;
}

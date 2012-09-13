/*
 * cxCreateProbeDataFromConfiguration.cpp
 *
 *  \date Nov 26, 2010
 *      \author christiana
 */

#include "cxCreateProbeDataFromConfiguration.h"
#include "sscTypeConversions.h"
#include <iostream>

ProbeXmlConfigParser::Configuration createConfigurationFromProbeData(ProbeXmlConfigParser::Configuration basis, ssc::ProbeData data)
{
	ProbeXmlConfigParser::Configuration config = basis;
//	config.mConfigId = uid;
//	config.mName = name;

	QSize storedSize(basis.mImageWidth, basis.mImageHeight);
	if (storedSize!=data.getImage().mSize)
	{
		// wrong size: resample
		data.resample(storedSize);
	}

	config.mLeftEdge = data.getImage().mClipRect_p[0];
	config.mRightEdge = data.getImage().mClipRect_p[1];
	config.mTopEdge = data.getImage().mClipRect_p[2];
	config.mBottomEdge = data.getImage().mClipRect_p[3];

	config.mOriginCol = data.getImage().mOrigin_p[0];
	config.mOriginRow = data.getImage().mOrigin_p[1];

	config.mPixelWidth = data.getImage().mSpacing[0];
	config.mPixelHeight = data.getImage().mSpacing[1];

	config.mImageWidth = data.getImage().mSize.width();
	config.mImageHeight = data.getImage().mSize.height();

	if (data.getType()==ssc::ProbeData::tSECTOR)
	{
		config.mWidthDeg = data.getWidth() / M_PI*180.0;
		config.mOffset = data.getDepthStart() / data.getImage().mSpacing[1];
		config.mDepth = (data.getDepthEnd() - data.getDepthStart()) / data.getImage().mSpacing[1];
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

ssc::ProbeData createProbeDataFromConfiguration(ProbeXmlConfigParser::Configuration config)
{
//	std::cout << "createProbeDataFromConfiguration()" << std::endl;
  if(config.isEmpty())
    return ssc::ProbeData();

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
	double depthStart = config.mOffset * config.mPixelHeight;
	double depthEnd = config.mDepth * config.mPixelHeight + depthStart;

	double width = config.mWidthDeg * M_PI / 180.0;//width in radians
	probeSector = ssc::ProbeData(ssc::ProbeData::tSECTOR);
	probeSector.setSector(depthStart, depthEnd, width);
  }
  else //Linear probe
  {
    int widtInPixels = config.mRightEdge - config.mLeftEdge;
    double width = config.mPixelWidth * double(widtInPixels); //width in mm
    // correct for top/bottom edges if applicable
    double depthStart = double(config.mTopEdge-config.mOriginRow) * config.mPixelHeight;
    double depthEnd = double(config.mBottomEdge-config.mOriginRow) * config.mPixelHeight;

//    probeSector = ssc::ProbeData(ssc::ProbeData::tLINEAR, depthStart, depthEnd, width);
	probeSector = ssc::ProbeData(ssc::ProbeData::tLINEAR);
	probeSector.setSector(depthStart, depthEnd, width);
  }

  probeSector.setImage(imageData);
  probeSector.setTemporalCalibration(config.mTemporalCalibration);

//  std::cout << "Probe sector for " << config.mConfigId << std::endl;
//  std::cout << streamXml2String(probeSector) << std::endl;

  return probeSector;
}

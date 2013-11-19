#include "cxCreateProbeDataFromConfiguration.h"
#include "sscTypeConversions.h"
#include <iostream>

namespace cx
{

ProbeXmlConfigParser::Configuration createConfigurationFromProbeData(ProbeXmlConfigParser::Configuration basis, ProbeData data)
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

	if (data.getType()==ProbeData::tSECTOR)
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

ProbeData createProbeDataFromConfiguration(ProbeXmlConfigParser::Configuration config)
{
  if(config.isEmpty())
    return ProbeData();

	ProbeData probeData;

  if (config.mWidthDeg > 0.1) // Sector probe
  {
	double depthStart = config.mOffset * config.mPixelHeight;
	double depthEnd = config.mDepth * config.mPixelHeight + depthStart;

	double width = config.mWidthDeg * M_PI / 180.0;//width in radians
	probeData = ProbeData(ProbeData::tSECTOR);
	probeData.setSector(depthStart, depthEnd, width);
  }
  else //Linear probe
  {
    int widtInPixels = config.mRightEdge - config.mLeftEdge;
    double width = config.mPixelWidth * double(widtInPixels); //width in mm
    // correct for top/bottom edges if applicable
    double depthStart = double(config.mTopEdge-config.mOriginRow) * config.mPixelHeight;
    double depthEnd = double(config.mBottomEdge-config.mOriginRow) * config.mPixelHeight;

	probeData = ProbeData(ProbeData::tLINEAR);
	probeData.setSector(depthStart, depthEnd, width);
  }

	probeData.setSpacing(Vector3D(config.mPixelWidth, config.mPixelHeight, 1));
	probeData.setSize(QSize(config.mImageWidth, config.mImageHeight));
	probeData.setOrigin_p(Vector3D(config.mOriginCol, config.mOriginRow, 0));
	probeData.setClipRect_p(DoubleBoundingBox3D(config.mLeftEdge,config.mRightEdge,config.mTopEdge,config.mBottomEdge,0,0));
	probeData.setTemporalCalibration(config.mTemporalCalibration);

	return probeData;
}

} // namespace cx

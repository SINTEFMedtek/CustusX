/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFRAME_H_
#define CXFRAME_H_

#include "cxGrabberExport.h"

namespace cx
{

/**
 * \ingroup cx_resource_videoserver
 */
class cxGrabber_EXPORT Frame
{
public:
  bool mNewStatus;
  double mTimestamp; ///< Timestamp in seconds since 1/1/1970 (epoch)
  int mWidth; ///< Width in pixels
  int mHeight; ///< Height in pixels
  int mPixelFormat; ///< Pixel format in OSType (FourCC)
  unsigned char* mFirstPixel; ///< Pointer to first pixel in frame
	double mSpacing[2];
  float mOrigin[2];
  int ulx;
  int uly;
  int urx;
  int ury;
  int brx;
  int bry;
  int blx;
  int bly;
  std::string probeName;
  int mImagingDepth; //Imaging depth in mm
  int mSectorSizeInPercent; //Size of sector in percent compared to full

	Frame():
		mNewStatus(false),
		mTimestamp(0.0),
		mWidth(0),
		mHeight(0),
		mPixelFormat(0),
		mFirstPixel(NULL),
		mSpacing(),
		mOrigin(),
		ulx(0),
		uly(0),
		urx(0),
		ury(0),
		brx(0),
		bry(0),
		blx(0),
		bly(0),
		mImagingDepth(0),
		mSectorSizeInPercent(0) {}
};
}//namespace cx

#endif //CXFRAME_H_

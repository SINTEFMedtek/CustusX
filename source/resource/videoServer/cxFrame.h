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

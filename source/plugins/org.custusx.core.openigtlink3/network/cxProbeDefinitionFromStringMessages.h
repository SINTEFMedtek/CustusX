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

#ifndef CXPROBEDEFINITIONFROMSTRINGMESSAGES_H
#define CXPROBEDEFINITIONFROMSTRINGMESSAGES_H

#include "igtlioLogic.h"
#include <boost/shared_ptr.hpp>
#include "cxProbeDefinition.h"
#include "cxImage.h"
#include "cxVector3D.h"

namespace cx
{

typedef boost::shared_ptr<struct SectorInfo> SectorInfoPtr;

const int tooLarge = 100000;

struct SectorInfo
{
	int mProbeType; //0 = unknown, 1 = sector, 2 = linear
	double mStartDepth;
	double mStopDepth;
	double mStartLineX;
	double mStartLineY;
	double mStopLineX;
	double mStopLineY;
	double mStartLineAngle;
	double mStopLineAngle;
	double mSpacingX;
	double mSpacingY;
	int mSectorLeftPixels;
	int mSectorRightPixels;
	int mSectorTopPixels;
	int mSectorBottomPixels;
	double mSectorLeftMm;
	double mSectorRightMm;
	double mSectorTopMm;
	double mSectorBottomMm;
	ImagePtr mImage;

	SectorInfo()
	{
		reset();
	}
	void reset()
	{
		mProbeType = tooLarge;
		mStartDepth = tooLarge;
		mStopDepth = tooLarge;
		mStartLineX = tooLarge;
		mStartLineY = tooLarge;
		mStopLineX = tooLarge;
		mStopLineY = tooLarge;
		mStartLineAngle = tooLarge;
		mStopLineAngle = tooLarge;
		mSpacingX = tooLarge;
		mSpacingY = tooLarge;
		mSectorLeftPixels = tooLarge;
		mSectorRightPixels = tooLarge;
		mSectorTopPixels = tooLarge;
		mSectorBottomPixels = tooLarge;
		mSectorLeftMm = tooLarge;
		mSectorRightMm = tooLarge;
		mSectorTopMm = tooLarge;
		mSectorBottomMm = tooLarge;
		mImage = ImagePtr();
	}
	bool isValid()
	{
		bool retval = true;
		retval = retval && mImage;
//		return retval;//test
		retval = retval && ((mProbeType == 1) || (mProbeType == 2));
		retval = retval && (mStopDepth < tooLarge);
		retval = retval && (mStartDepth < tooLarge);
		retval = retval && !similar(mStopDepth - mStartDepth, 0);

		retval = retval && (mStartLineX < tooLarge);
		retval = retval && (mStopLineX < tooLarge);
		retval = retval && (mStartLineY < tooLarge);
		retval = retval && (mStopLineY < tooLarge);
		retval = retval && !similar(fabs(mStartLineX - mStopLineX), 0);

		retval = retval && (mStartLineAngle < tooLarge);
		retval = retval && (mStopLineAngle < tooLarge);
		retval = retval && !similar(fabs(mStopLineAngle - mStartLineAngle), 0);

		retval = retval && (mSpacingX < tooLarge);
		retval = retval && (mSpacingY < tooLarge);
		retval = retval && !similar(mSpacingX, 0);
		retval = retval && !similar(mSpacingY, 0);

		retval = retval && (mSectorRightPixels < tooLarge);
		retval = retval && (mSectorLeftPixels < tooLarge);
		retval = retval && (mSectorTopPixels < tooLarge);
		retval = retval && (mSectorBottomPixels < tooLarge);
		retval = retval && (mSectorRightPixels - mSectorLeftPixels != 0);
		retval = retval && (mSectorTopPixels - mSectorBottomPixels != 0);

		retval = retval && (mSectorRightMm < tooLarge);
		retval = retval && (mSectorLeftMm < tooLarge);
		retval = retval && (mSectorTopMm < tooLarge);
		retval = retval && (mSectorBottomMm < tooLarge);
		retval = retval && !similar(mSectorRightMm - mSectorLeftMm, 0);
		retval = retval && !similar(mSectorTopMm - mSectorBottomMm, 0);
		/*if(mProbeType == 0)//sector
		{
			retval = retval && (mStopLineAngle < tooLarge);
			retval = retval && (mStartLineAngle < tooLarge);
			retval = retval && !similar(fabs(mStopLineAngle - mStartLineAngle), 0);
		}*/

		return retval;
	}
};

typedef boost::shared_ptr<class ProbeDefinitionFromStringMessages> ProbeDefinitionFromStringMessagesPtr;

/**
 * Create a ProbeDefinition based on BK String messages from PLUS.
 *
 * \date May 03, 2017
 * \author Ole Vegard Solberg, SINTEF
 */
class ProbeDefinitionFromStringMessages
{
public:
	ProbeDefinitionFromStringMessages();
	void parseStringMessage(igtlio::BaseConverter::HeaderData header, QString message);
	void setImage(ImagePtr image);
	bool haveValidValues();
	ProbeDefinitionPtr createProbeDefintion(QString uid);

	void parseValue(QString name, QString value);

protected:
	ProbeDefinitionPtr mProbeDefinition;
	SectorInfoPtr mSectorInfo;
	bool mTestMode;

};

}//cx

#endif // CXPROBEDEFINITIONFROMSTRINGMESSAGES_H

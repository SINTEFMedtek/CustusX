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

struct SectorInfo
{
	int mProbeType; //0 = sector, 1 = Linear, 2 = unknown
	double mStartDepth;
	double mStopDepth;
	double mStartLineX;
	double mStartLineY;
	double mStopLineX;
	double mStopLineY;
	double mStartLineAngle;
	double mStopLineAngle;
	ImagePtr mImage;

	SectorInfo()
	{
		reset();
	}
	void reset()
	{
		mProbeType = 3;
		mStartDepth = 0;
		mStopDepth = 0;
		mStartLineX = 0;
		mStartLineY = 0;
		mStopLineX = 0;
		mStopLineY = 0;
		mStartLineAngle = 0;
		mStopLineAngle = 0;
		mImage = ImagePtr();
	}
	bool isValid()
	{
		bool retval = true;
		retval = retval && (mProbeType < 2);
		retval = retval && !similar(mStartDepth, 0);
		retval = retval && !similar(mStopDepth, 0);
		retval = retval && !similar(mStopDepth, 0);
		retval = retval && !similar(mStartLineX, 0);
		retval = retval && !similar(mStartLineY, 0);
		retval = retval && !similar(mStopLineX, 0);
		if(mProbeType == 0)//sector
		{
			retval = retval && !similar(mStartLineAngle, 0);
			retval = retval && !similar(mStopLineAngle, 0);
		}
		retval = retval && mImage;

		return retval;
	}
};

typedef boost::shared_ptr<class ProbeDefinitionFromStringMessages> ProbeDefinitionFromStringMessagesPtr;

class ProbeDefinitionFromStringMessages
{
public:
	ProbeDefinitionFromStringMessages();
	void parseStringMessage(igtlio::BaseConverter::HeaderData header, QString message);
	void setImage(ImagePtr image);
	bool haveValidValues();
	ProbeDefinitionPtr createProbeDefintion(QString uid);

protected:
	ProbeDefinitionPtr mProbeDefinition;
	SectorInfo mSectorInfo;
	bool mTestMode;

};

}//cx

#endif // CXPROBEDEFINITIONFROMSTRINGMESSAGES_H

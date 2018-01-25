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

#include "org_custusx_core_openigtlink3_Export.h"
#include "igtlioLogic.h"
#include <boost/shared_ptr.hpp>
#include "cxProbeDefinition.h"
#include "cxImage.h"
#include "cxVector3D.h"

namespace cx
{

typedef boost::shared_ptr<struct SectorInfo> SectorInfoPtr;


struct SectorInfo
{
	const int tooLarge = 100000;

	int mProbeType; //0 = unknown, 1 = sector, 2 = linear

	ImagePtr mImage;

	//Spacing are sent as separate messages, should be sent with image in the future.
	double mSpacingX;
	double mSpacingY;

	//new standard
	std::vector<double> mOrigin;
	std::vector<double> mAngles;
	std::vector<double> mBouningBox;
	std::vector<double> mDepths;
	double mLinearWidth;

	bool mHaveChanged;

	SectorInfo()
	{
		reset();
	}
	void reset()
	{
		mHaveChanged = true;
		mProbeType = tooLarge;

		//new standard
		mOrigin.clear();
		mAngles.clear();
		mBouningBox.clear();
		mDepths.clear();
		mLinearWidth = tooLarge;

		mSpacingX = tooLarge;
		mSpacingY = tooLarge;

		mImage = ImagePtr();
	}
	bool isValid()
	{
		bool retval = true;
		retval = retval && mImage;
		retval = retval && ((mProbeType == 1) || (mProbeType == 2));
		retval = retval && (mOrigin.size() == 3);
		retval = retval && ((mAngles.size() == 2) || (mAngles.size() == 4));//2D == 2, 3D == 4
		retval = retval && ((mBouningBox.size() == 4) || (mBouningBox.size() == 6)); //2D == 4, 3D == 6
		retval = retval && (mDepths.size() == 2);
		if(mProbeType == 2)//linar
			retval = retval && (mLinearWidth < tooLarge);//Only for linear probes

		//Send spacing for now. Try to send it as image spacing
		retval = retval && (mSpacingX < tooLarge);
		retval = retval && (mSpacingY < tooLarge);
		retval = retval && !similar(mSpacingX, 0);
		retval = retval && !similar(mSpacingY, 0);

		return retval;
	}

	bool haveChanged()
	{
		return mHaveChanged;
	}

};

typedef boost::shared_ptr<class ProbeDefinitionFromStringMessages> ProbeDefinitionFromStringMessagesPtr;

/**
 * Create a ProbeDefinition based on BK String messages from PLUS.
 *
 * \date May 03, 2017
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_core_openigtlink3_EXPORT ProbeDefinitionFromStringMessages
{
public:
	ProbeDefinitionFromStringMessages();
	void parseStringMessage(igtlio::BaseConverter::HeaderData header, QString message);
	void setImage(ImagePtr image);
	bool haveValidValues();
	bool haveChanged();
	ProbeDefinitionPtr createProbeDefintion(QString uid);

	void parseValue(QString name, QString value);

protected:
	ProbeDefinitionPtr mProbeDefinition;
	SectorInfoPtr mSectorInfo;

private:
	std::vector<double> toDoubleVector(QString values, QString separator = QString(" "));
	DoubleBoundingBox3D getBoundinBox() const;
	double getWidth();
	ProbeDefinitionPtr initProbeDefinition();
	QSize getSize();
	double getBoundingBoxThirdDimensionStart() const;
	double getBoundingBoxThirdDimensionEnd() const;
};

}//cx

#endif // CXPROBEDEFINITIONFROMSTRINGMESSAGES_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPROBEDEFINITIONFROMSTRINGMESSAGES_H
#define CXPROBEDEFINITIONFROMSTRINGMESSAGES_H

#include "org_custusx_core_openigtlink3_Export.h"
#include "igtlioLogic.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include "cxProbeDefinition.h"
#include "cxImage.h"
#include "cxVector3D.h"

namespace cx
{

bool validSpacing(Vector3D spacing);

/**
 * Internal container for holding incoming variable values.
 *
 * Used by ProbeDefinitionFromStringMessages as a container for holding values
 * from string messages. When SectorInfo got a valid/complete data set
 * a ProbeDefinition can be created form these values.
 *
 * Defined here (rather than in the .cpp) so tests can subclass
 * ProbeDefinitionFromStringMessages and inspect mSectorInfo directly.
 */
struct SectorInfo
{
	const int tooLarge = 100000;

	ProbeDefinition::TYPE mProbeType; //0 = unknown, 1 = sector, 2 = linear

	ImagePtr mImage;

	//Spacing are sent as separate messages, should be sent with image in the future.
	double mSpacingX;
	double mSpacingY;
	double mSpacingZ;

	//new standard
	std::vector<double> mOrigin;
	std::vector<double> mAngles;
	std::vector<double> mBoundingBox;
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
		mProbeType = ProbeDefinition::tNONE;

		//new standard
		mOrigin.clear();
		mAngles.clear();
		mBoundingBox.clear();
		mDepths.clear();
		mLinearWidth = tooLarge;

		mSpacingX = tooLarge;
		mSpacingY = tooLarge;
		mSpacingZ = 1.0; //Spacing z may not be received

		mImage = ImagePtr();
	}
	bool isValid()
	{
		if(!mImage)
			return false;

		bool retval = true;
		retval = retval && mImage;
		retval = retval && ((mProbeType == ProbeDefinition::tSECTOR) || (mProbeType == ProbeDefinition::tLINEAR));
		retval = retval && (mOrigin.size() == 3);
		retval = retval && ((mAngles.size() == 2) || (mAngles.size() == 4));//2D == 2, 3D == 4
		retval = retval && ((mBoundingBox.size() == 4) || (mBoundingBox.size() == 6)); //2D == 4, 3D == 6
		retval = retval && (mDepths.size() == 2);
		if(mProbeType == ProbeDefinition::tLINEAR)
			retval = retval && (mLinearWidth < tooLarge);//Only for linear probes

		Vector3D spacing = mImage->getSpacing();
		if(!validSpacing(spacing))
		{
			retval = retval && (mSpacingX < tooLarge);
			retval = retval && (mSpacingY < tooLarge);
			retval = retval && !similar(mSpacingX, 0);
			retval = retval && !similar(mSpacingY, 0);
			retval = retval && !similar(mSpacingZ, 0);
		}

		return retval;
	}

	bool haveChanged()
	{
		return mHaveChanged;
	}
};

typedef boost::shared_ptr<SectorInfo> SectorInfoPtr;
typedef boost::shared_ptr<class ProbeDefinitionFromStringMessages> ProbeDefinitionFromStringMessagesPtr;

/**
 * Create a ProbeDefinition based on BK String messages from Plus.
 *
 * Currently ProbeDefinitionFromStringMessages creates a ProbeDefinition from BK String messages from Plus.
 * Later this should be meta data sent with OpenIGTLink (version 3) messages.
 * OpenIGTLinkIO should define this protocol.
 *
 * \date May 03, 2017
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_core_openigtlink3_EXPORT ProbeDefinitionFromStringMessages
{
public:
	ProbeDefinitionFromStringMessages();
	void reset();
	void parseStringMessage(igtlioBaseConverter::HeaderData header, QString message);
	void setImage(ImagePtr image);
	bool haveValidValues();
	bool haveChanged();
	ProbeDefinitionPtr createProbeDefintion(QString uid);

	void parseValue(QString name, QString value);

protected:
	SectorInfoPtr mSectorInfo;
	int mOriginalTime;

private:
	std::vector<double> toDoubleVector(QString values, QString separator = QString(" ")) const;
	DoubleBoundingBox3D getBoundingBox() const;
	double getWidth();
	ProbeDefinitionPtr initProbeDefinition();
	QSize getSize();
	double getBoundingBoxThirdDimensionStart() const;
	double getBoundingBoxThirdDimensionEnd() const;

	ProbeDefinitionPtr mProbeDefinition;
};

}//cx

#endif // CXPROBEDEFINITIONFROMSTRINGMESSAGES_H

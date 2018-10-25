/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxProbeDefinitionFromStringMessages.h"

#include <vtkXMLDataElement.h>
#include <vtkXMLUtilities.h>
#include <vtkImageData.h>
#include <igtlioUsSectorDefinitions.h>

#include "cxLogger.h"

namespace cx
{

/**
 * Internal container for holding incoming variable values.
 *
 * Used by ProbeDefinitionFromStringMessages as a container for holding values
 * from string messages. When SectorInfo got a valid/complete data set
 * a ProbeDefinition can be created form these values.
 */
struct SectorInfo
{
	const int tooLarge = 100000;

	ProbeDefinition::TYPE mProbeType; //0 = unknown, 1 = sector, 2 = linear

	ImagePtr mImage;

	//Spacing are sent as separate messages, should be sent with image in the future.
	double mSpacingX;
	double mSpacingY;

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

		mImage = ImagePtr();
	}
	bool isValid()
	{
		bool retval = true;
		retval = retval && mImage;
		retval = retval && ((mProbeType == ProbeDefinition::tSECTOR) || (mProbeType == ProbeDefinition::tLINEAR));
		retval = retval && (mOrigin.size() == 3);
		retval = retval && ((mAngles.size() == 2) || (mAngles.size() == 4));//2D == 2, 3D == 4
		retval = retval && ((mBoundingBox.size() == 4) || (mBoundingBox.size() == 6)); //2D == 4, 3D == 6
		retval = retval && (mDepths.size() == 2);
		if(mProbeType == ProbeDefinition::tLINEAR)
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


ProbeDefinitionFromStringMessages::ProbeDefinitionFromStringMessages() :
	mSectorInfo(new SectorInfo)
{}

void ProbeDefinitionFromStringMessages::reset()
{
	mSectorInfo->reset();
}

void ProbeDefinitionFromStringMessages::parseStringMessage(igtlioBaseConverter::HeaderData header, QString message)
{
	QString name = QString(header.deviceName.c_str());
	QString value = message;
	this->parseValue(name, value);
}

/**
 * @brief ProbeDefinitionFromStringMessages::toDoubleVector Converts a string with a separator to a double vector.
 * This function is the counterpart to PlusCommon::ToString() in Plus
 *
 * If needed elsewere the function can be moved to a common place an used as an utility funciton.
 *
 * @param values String with multiple double values
 * @param separator The separator between the values used in the string
 * @return Vector of doubles
 */
std::vector<double> ProbeDefinitionFromStringMessages::toDoubleVector(QString values, QString separator) const
{
	std::vector<double> retval;
	QStringList valueList = values.split(separator);
	for (int i = 0; i < valueList.size(); ++i)
	{
		double doublevalue = valueList[i].toDouble();
		retval.push_back(doublevalue);
	}
	return retval;
}

void ProbeDefinitionFromStringMessages::parseValue(QString name, QString value)
{
	int intValue = value.toInt();
	double doubleValue = value.toDouble();
	std::vector<double> doubleVector = toDoubleVector(value);

//	CX_LOG_DEBUG() << "parseStringMessage: "	<< " name: " << name
//				   << " intValue: " << intValue
//				   << " doubleValue: " << doubleValue;

	if (name == IGTLIO_KEY_PROBE_TYPE)
	{
		if (mSectorInfo->mProbeType != intValue)
		{
			mSectorInfo->mProbeType = static_cast<ProbeDefinition::TYPE>(intValue);
		}
	}
	//New standard
	else if (name == IGTLIO_KEY_ORIGIN)
	{
		if(mSectorInfo->mOrigin != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mOrigin = doubleVector;
		}
	}
	else if (name == IGTLIO_KEY_ANGLES)
	{
		if(mSectorInfo->mAngles != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mAngles = doubleVector;
		}
	}
	else if (name == IGTLIO_KEY_BOUNDING_BOX)
	{
		if(mSectorInfo->mBoundingBox != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mBoundingBox = doubleVector;
		}
	}
	else if (name == IGTLIO_KEY_DEPTHS)
	{
		if(mSectorInfo->mDepths != doubleVector)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mDepths = doubleVector;
		}
	}
	else if (name == IGTLIO_KEY_LINEAR_WIDTH)
	{
		if(mSectorInfo->mLinearWidth != doubleValue)
		{
			mSectorInfo->mHaveChanged  = true;
			mSectorInfo->mLinearWidth = doubleValue;
		}
	}
	else if (name == IGTLIO_KEY_SPACING_X)
	{
		mSectorInfo->mSpacingX = doubleValue;
	}
	else if (name == IGTLIO_KEY_SPACING_Y)
	{
		mSectorInfo->mSpacingY = doubleValue;
	}
}

void ProbeDefinitionFromStringMessages::setImage(ImagePtr image)
{
	mSectorInfo->mImage = image;
}

bool ProbeDefinitionFromStringMessages::haveValidValues()
{
	return mSectorInfo->isValid();
}

bool ProbeDefinitionFromStringMessages::haveChanged()
{
	return mSectorInfo->haveChanged();
}

ProbeDefinitionPtr ProbeDefinitionFromStringMessages::createProbeDefintion(QString uid)
{
	mSectorInfo->mHaveChanged = false;

	if(!this->haveValidValues())
		return ProbeDefinitionPtr();

	//Send spacing as messages for now. Should be sent together with image.
	mSectorInfo->mImage->getBaseVtkImageData()->SetSpacing(mSectorInfo->mSpacingX, mSectorInfo->mSpacingY, 1.0);
	Vector3D spacing = mSectorInfo->mImage->getSpacing();
	Vector3D origin_p(mSectorInfo->mOrigin[0], mSectorInfo->mOrigin[1], mSectorInfo->mOrigin[2]);

	ProbeDefinitionPtr probeDefinition = this->initProbeDefinition();
	probeDefinition->setUid(uid);
	probeDefinition->setOrigin_p(origin_p);
	probeDefinition->setSpacing(spacing);
	probeDefinition->setClipRect_p(this->getBoundinBox());
	probeDefinition->setSector(mSectorInfo->mDepths[0], mSectorInfo->mDepths[1], this->getWidth());
	probeDefinition->setSize(this->getSize());
	probeDefinition->setUseDigitalVideo(true);

	return probeDefinition;
}

ProbeDefinitionPtr ProbeDefinitionFromStringMessages::initProbeDefinition()
{
	ProbeDefinitionPtr probeDefinition;
	probeDefinition = ProbeDefinitionPtr(new ProbeDefinition(mSectorInfo->mProbeType));

	if (mSectorInfo->mProbeType == ProbeDefinition::tNONE)
	{
		CX_LOG_ERROR() << "ProbeDefinitionFromStringMessages::initProbeDefinition: Incorrect probe type: " << mSectorInfo->mProbeType;
	}
	return probeDefinition;
}

double ProbeDefinitionFromStringMessages::getWidth()
{
	double width = 0;
	if(mSectorInfo->mProbeType == ProbeDefinition::tLINEAR)
	{
		width = mSectorInfo->mLinearWidth;
	}
	else if (mSectorInfo->mProbeType == ProbeDefinition::tSECTOR)
	{
		width = mSectorInfo->mAngles[1] - mSectorInfo->mAngles[0];
	}
	return width;
}

QSize ProbeDefinitionFromStringMessages::getSize()
{
	Eigen::Array3i dimensions(mSectorInfo->mImage->getBaseVtkImageData()->GetDimensions());
	QSize size(dimensions[0], dimensions[1]);
	return size;
}

DoubleBoundingBox3D ProbeDefinitionFromStringMessages::getBoundinBox() const
{
	DoubleBoundingBox3D retval(mSectorInfo->mBoundingBox[0], mSectorInfo->mBoundingBox[1],
			mSectorInfo->mBoundingBox[2], mSectorInfo->mBoundingBox[3],
			this->getBoundingBoxThirdDimensionStart(),
			this->getBoundingBoxThirdDimensionEnd());
	return retval;
}

double ProbeDefinitionFromStringMessages::getBoundingBoxThirdDimensionStart() const
{
	if(mSectorInfo->mBoundingBox.size() == 6)
		return mSectorInfo->mBoundingBox[4];
	else
		return 0;
}

double ProbeDefinitionFromStringMessages::getBoundingBoxThirdDimensionEnd() const
{
	if(mSectorInfo->mBoundingBox.size() == 6)
		return mSectorInfo->mBoundingBox[5];
	else
		return 0;
}

}//cx

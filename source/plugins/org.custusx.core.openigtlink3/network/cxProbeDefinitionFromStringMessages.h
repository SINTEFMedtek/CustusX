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
#include "cxProbeDefinition.h"
#include "cxImage.h"
#include "cxVector3D.h"

namespace cx
{

typedef boost::shared_ptr<struct SectorInfo> SectorInfoPtr;
typedef boost::shared_ptr<class ProbeDefinitionFromStringMessages> ProbeDefinitionFromStringMessagesPtr;

/**
 * Create a ProbeDefinition based on BK String messages from PLUS.
 *
 * Currently ProbeDefinitionFromStringMessages creates a ProbeDefinition from BK String messages from PLUS.
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
	void parseStringMessage(igtlio::BaseConverter::HeaderData header, QString message);
	void setImage(ImagePtr image);
	bool haveValidValues();
	bool haveChanged();
	ProbeDefinitionPtr createProbeDefintion(QString uid);

	void parseValue(QString name, QString value);

protected:
	SectorInfoPtr mSectorInfo;

private:
	std::vector<double> toDoubleVector(QString values, QString separator = QString(" ")) const;
	DoubleBoundingBox3D getBoundinBox() const;
	double getWidth();
	ProbeDefinitionPtr initProbeDefinition();
	QSize getSize();
	double getBoundingBoxThirdDimensionStart() const;
	double getBoundingBoxThirdDimensionEnd() const;

	ProbeDefinitionPtr mProbeDefinition;
};

}//cx

#endif // CXPROBEDEFINITIONFROMSTRINGMESSAGES_H

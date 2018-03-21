/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIGTLINKCONVERSIONBASE_H
#define CXIGTLINKCONVERSIONBASE_H

#include "igtlMessageBase.h"
#include <QDateTime>
#include "cxOpenIGTLinkUtilitiesExport.h"


namespace cx
{

/** Convert utilities for igtl::MessageBase
 *
 * encode: create igtl messages
 * decode: read from igtl messages
 *
 * decode methods assume Unpack() has been called.
 */
class cxOpenIGTLinkUtilities_EXPORT IGTLinkConversionBase
{
public:
	void encode_timestamp(QDateTime ts, igtl::MessageBase* msg);
	QDateTime decode_timestamp(igtl::MessageBase *msg);

private:

};

} //namespace cx

#endif // CXIGTLINKCONVERSIONBASE_H

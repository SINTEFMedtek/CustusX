/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDITOOLSETUP_H_
#define CXNDITOOLSETUP_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

#include <map>
#include <vector>
#include <QStringList>
#include "cxNdiDevice.h"

namespace cx
{

/** What the tool setup needs to know about a configured tool. */
struct org_custusx_core_tracking_system_ndi_EXPORT NdiToolConfig
{
	QString uid;
	bool reference = false;
	bool wireless = false;  ///< Polaris passive tool, identified by its .rom file
	QString sromFile;       ///< Tool definition, required for wireless tools
	int port = -1;          ///< 0-based physical port, wired (Aurora) tools
	int channel = 0;        ///< Channel on a splitter, 5DOF tools only
	bool fiveDOF = false;
};

struct org_custusx_core_tracking_system_ndi_EXPORT NdiToolSetupResult
{
	std::map<int, QString> portHandleToUid; ///< The port handle of each tool that was set up
	QStringList warnings;
	QStringList messages;
	bool success() const { return !portHandleToUid.empty(); }
};

/** \brief Prepare the device's port handles for the configured tools.
 *
 * Same sequence as PLUS:
 * 1. Free port handles waiting to be freed.
 * 2. Wireless tools: request a port handle each and load its .rom file.
 * 3. Initialize and enable port handles, repeated until there are no new ones, because
 *    the second sensor on a splitter only appears after the first one is enabled.
 * 4. Wired tools: match each enabled port handle to a tool by its physical port and
 *    channel (see NdiDevice::getPortLocation).
 *
 * \ingroup org_custusx_core_tracking_system_ndi
 */
org_custusx_core_tracking_system_ndi_EXPORT NdiToolSetupResult setupNdiTools(NdiDevice& device, const std::vector<NdiToolConfig>& tools);

} // namespace cx

#endif /* CXNDITOOLSETUP_H_ */

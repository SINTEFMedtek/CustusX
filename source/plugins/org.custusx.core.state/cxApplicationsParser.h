/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXAPPLICATIONSPARSER_H
#define CXAPPLICATIONSPARSER_H

#include "org_custusx_core_state_Export.h"
#include "cxXmlOptionItem.h"
#include "cxStateService.h"

namespace cx
{

/**
 * \ingroup org_custusx_core_state
 * \date 2010-08-04
 * \author Christian Askeland, SINTEF
 *
 */
class org_custusx_core_state_EXPORT ApplicationsParser
{
public:
	ApplicationsParser();
	~ApplicationsParser() {}

	Desktop getDefaultDesktop(QString workflowName);
	Desktop getDesktop(QString workflowName);
	void setDesktop(QString workflowName, Desktop desktop);
	void resetDesktop(QString workflowName);

private:
	void addDefaultDesktops(QString workflowStateUid, QString layoutUid, QString mainwindowstate);
	XmlOptionFile getSettings();
	//	XmlOptionFile mXmlFile;
	std::map<QString, Desktop> mWorkflowDefaultDesktops;
	void addToolbarsToDesktop(Desktop& desktop, QStringList toolbars);
};

}

#endif // CXAPPLICATIONSPARSER_H

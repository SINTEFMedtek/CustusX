/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRAIDIONICS_H
#define CXRAIDIONICS_H


#include "cxGenericScriptFilter.h"

namespace cx
{

class Raidionics
{
public:
	Raidionics();

	static QString raidionicsCommandString(CommandStringVariables variables);
	static QString createRaidionicsIniFile(CommandStringVariables variables);
	static void createRaidionicsJasonFile(QString jsonFilePath);
};
}//cx
#endif // CXRAIDIONICS_H

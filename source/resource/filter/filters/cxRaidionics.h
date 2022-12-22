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

class cxResourceFilter_EXPORT Raidionics
{
public:
	Raidionics(CommandStringVariables variables, QStringList targets);

	QString raidionicsCommandString();
	QString getOutputFolder();

protected:
	QString createRaidionicsIniFile();
	void createRaidionicsJasonFile(QString jsonFilePath);
	QString copyInputFiles(QString parentFolder, QString inputFileName, QString subfolder);
	QString getModelFolder();
	static QString subfolderT0() {return "T0";}

	CommandStringVariables mVariables;
	QString mOutputFolder;
	QStringList mTargets;
};

typedef boost::shared_ptr<class Raidionics> RaidionicsPtr;
}//cx
#endif // CXRAIDIONICS_H

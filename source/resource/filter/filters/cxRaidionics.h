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
	Raidionics(VisServicesPtr services, CommandStringVariables variables, QStringList targets);

	QString raidionicsCommandString();
	QString getOutputFolder();
	QString getTempFolder();
	static QString getIniFileName() {return "Raidionics.ini";}
	static QString getJsonFileName() {return "Raidionics.json";}
	static QString getRaidionicsCTFileNamePrefix() {return "input_ct_gd_";}
	QString getRadionicsInputFileName(QString inputFile);

protected:
	QString createRaidionicsIniFile();
	void createRaidionicsJasonFile(QString jsonFilePath);
	QString copyInputFiles(QString inputFileName, QString subfolder);
	QString getModelFolder();
	static QString subfolderT0() {return "T0";}
	QJsonArray createTargetArray(QString target);

	VisServicesPtr mServices;
	CommandStringVariables mVariables;
	QString mOutputFolder;
	QString mTempFolder;
	QStringList mTargets;
};

typedef boost::shared_ptr<class Raidionics> RaidionicsPtr;
}//cx
#endif // CXRAIDIONICS_H

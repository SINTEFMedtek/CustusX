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
	QStringList setOutputClasses(QStringList outputClasses);
	//Utility functions
	static QStringList expandOutputClasses(QStringList targetList);
	static QString colorForLungClass(QString outputClass);
	static QStringList createTargetList(QString target);

protected:
	QString createRaidionicsIniFile();
	void createRaidionicsJasonFile(QString jsonFilePath);
	QString copyInputFiles(QString inputFileName, QString subfolder);
	QString getModelFolder();
	static QString subfolderT0() {return "T0";}
	bool useFormatThresholding(QString target);
	QString targetDescription(QString target);
	QJsonArray createTargetArray(QString target);
	static QString getTarget(ORGAN_TYPE organType);
	static ORGAN_TYPE getOrganType(QString target);

	VisServicesPtr mServices;
	CommandStringVariables mVariables;
	QString mOutputFolder;
	QString mTempFolder;
	QStringList mTargets;
};

typedef boost::shared_ptr<class Raidionics> RaidionicsPtr;
}//cx
#endif // CXRAIDIONICS_H

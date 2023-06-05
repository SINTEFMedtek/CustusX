/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXGENERICSCRIPTFILTER_H
#define CXGENERICSCRIPTFILTER_H

#include "cxFilterImpl.h"
#include "cxSettings.h"
#include "cxProcessWrapper.h"
#include <QColor>
#include "cxSelectDataStringProperty.h"


namespace cx
{
typedef boost::shared_ptr<class Raidionics> RaidionicsPtr;

struct cxResourceFilter_EXPORT CommandStringVariables
{
	QString inputFilePath;
	QString outputFilePath;
	QString envPath;
	QString scriptFilePath;
	QString cArguments;
	QString scriptEngine;
	QString model;

	CommandStringVariables(QString parameterFilePath, ImagePtr input);
};

struct cxResourceFilter_EXPORT OutputVariables
{
	bool mCreateOutputVolume = false;
	bool mCreateOutputMesh = false;
	QStringList mOutputColorList;
	QStringList mOutputClasses;
	bool mValid = false;

	OutputVariables();
	OutputVariables(QString parameterFilePath);
};

/** Generic filter calling external filter script.
 *
 *
 * \ingroup cx_resource_filter
 * \date Mar 10, 2020
 * \author Torgrim Lie
 */

class cxResourceFilter_EXPORT GenericScriptFilter : public FilterImpl
{
	Q_OBJECT
public:
	GenericScriptFilter(VisServicesPtr services);
	virtual ~GenericScriptFilter();

	enum SCRIPT_ENGINE
	{
		seUnknown,
		seStandard,
		seDeepSintef,
		seRaidionics,
		seCOUNT
	};

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	FilePathPropertyPtr getParameterFile(QDomElement root);
	void setParameterFilePath(QString path);
	FilePreviewPropertyPtr getIniFileOption(QDomElement root);
	PatientModelServicePtr mPatientModelService;
	void setOutputClasses(QStringList outputClasses);

signals:
	void launchDialog(QString venvPath, QString createCommand, QString command);
public slots:
	void launchDialogSlot(QString venvPath, QString createCommand, QString command);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
	QString createCommandString(ImagePtr input);
	bool runCommandStringAndWait(QString command);
	QString getCustomPath();
	void setupOutputColors(QStringList colorList);
	QColor createColor(QStringList color);
	QColor getDefaultColor();
	void createOutputMesh(QColor color);
	bool readGeneratedSegmentationFiles(bool createOutputVolume, bool createOutputMesh);
	QString createImageName(QString parentName, QString filePath);
	void createOutputVolume();
	void deleteNotUsedFiles(QString fileNameMhd, bool createOutputVolume);
	QString getScriptPath();
	QString getInputFilePath(ImagePtr input);
	QString getOutputFilePath(ImagePtr input);

	CommandStringVariables createCommandStringVariables(ImagePtr input);
	QString standardCommandString(CommandStringVariables variables);
	QString findScriptFile(QString path);
	bool isUsingRaidionicsEngine();
	QString deepSintefCommandString(CommandStringVariables variables);
	
	bool environmentExist(QString path);
	QString getEnvironmentPath(CommandStringVariables variables);
	QString getEnvironmentBasePath(QString environmentPath);
	QString findRequirementsFileLocation(QString path);
	bool createVirtualPythonEnvironment(QString environmentPath, QString requirementsPath, QString createScript = QString(), QString command = QString());
	bool isVirtualEnvironment(QString path);
	QString getFixedEnvironmentSubdir();
	QString removeTrailingPythonVariable(QString environmentPath);
	bool showVenvInfoDialog(QString venvPath, QString createCommand);
	bool createVenv(QString createCommand, QString command);
	bool setScriptEngine(CommandStringVariables variables);
	bool initRaidionicsEngine(CommandStringVariables variables);
	void setOutputColorsFromClasses();
	int getClassNumber(QString filePath);
	ORGAN_TYPE getOrganType(int classNumber);

	FilePathPropertyPtr mScriptFile;
	FilePreviewPropertyPtr mScriptFilePreview;
	OutputVariables mOutputVariables;

	vtkImageDataPtr mRawResult;
	QString mOutputChannelName;
	QString mScriptPathAddition;
	ProcessWrapperPtr mCommandLine;
	QString mResultFileEnding;
	QStringList mOutoutOrgans;
	ImagePtr mOutputImage;
	QList<QColor> mOutputColors;
	QStringList mOutputClasses;
	QStringList mOutputColorList;

	SelectDataStringPropertyBasePtr mOutputImageSelectDataPtr;
	StringPropertySelectMeshPtr mOutputMeshSelectMeshPtr;
	BoolPropertyPtr mOutputMeshOption;
	SCRIPT_ENGINE mScriptEngine = seUnknown;
	RaidionicsPtr mRaidionicsUtilities = nullptr;

protected slots:
	void scriptFileChanged();
	void processStateChanged();
	void processFinished(int code, QProcess::ExitStatus status);
	void processError(QProcess::ProcessError error);
	void processReadyRead();
	void processReadyReadError();
	bool createProcess();
	bool deleteProcess();
	bool disconnectProcess();
};
typedef boost::shared_ptr<class GenericScriptFilter> GenericScriptFilterPtr;


} // namespace cx



#endif // CXGENERICSCRIPTFILTER_H

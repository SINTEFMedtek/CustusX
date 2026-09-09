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
#include <QMap>
#include <QMutex>
#include <QAtomicInt>
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
	QStringList mCreateOutputVolumeList;
	QStringList mCreateOutputMeshList;
	QList<int> mSmoothingSettingList;
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
		seRaidionics,
		seTotalSegmentator,
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
	// Sends SIGTERM, not SIGKILL: the script relies on receiving it to
	// terminate any child process it spawned (e.g. a TotalSegmentator
	// subprocess) - SIGKILL would leave such a child orphaned.
	void requestStop();
	// Appended (space-separated) after the .ini file's own "arguments"
	// value, becoming one extra element of the script's own sys.argv.
	void setExtraCommandLineArguments(QString args);
	// Set in the launched process' environment, in addition to (or
	// overriding) the inherited system environment.
	void setExtraEnvironmentVariable(QString name, QString value);

signals:
	void scriptOutput(const QString& line);
	void meshGenerationProgress(int percent);
	void launchDialog(QString venvPath, QString createCommand, QString command);
public slots:
	void launchDialogSlot(QString venvPath, QString createCommand, QString command);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
	QString createCommandString(ImagePtr input);
	QString updateEnvPathIfWindows(QString envPath);
	QString updateScriptFilePathIfWindows(QString envPath, QString scriptFilePath);
	bool runCommandStringAndWait(QString command);
	void setupOutputColors(QStringList colorList);
	QColor createColor(QStringList color);
	QColor getDefaultColor();
	void createOutputMesh(QColor color, int smoothing = 1);
	vtkPolyDataPtr contourFilter(int smoothing);
	bool readGeneratedSegmentationFiles(QStringList createOutputVolume, QStringList createOutputMesh);
	QString createImageName(QString parentName, QString filePath);
	int countPlannedMeshes(QStringList createOutputMeshList) const;
	void appendToLineBuffer(const QString& newData);
	void createOutputVolume();
	void deleteNotUsedFiles(QString fileNameMhd, bool createOutputVolume);
	QString getScriptPath();
	QString getInputFilePath(ImagePtr input);
	QString getOutputFilePath(ImagePtr input);
	ProcessWrapperPtr getCommandLine();
	void setCommandLine(ProcessWrapperPtr commandLine);

	CommandStringVariables createCommandStringVariables(ImagePtr input);
	QString standardCommandString(CommandStringVariables variables);
	QString findScriptFile(QString path);
	bool isUsingRaidionicsEngine();
	bool environmentExist(QString path);
	QString getEnvironmentPath(CommandStringVariables variables);
	QString getEnvironmentBasePath(QString environmentPath);
	QString findRequirementsFileLocation(QString path);
	bool createVirtualPythonEnvironment(QString environmentPath, QString requirementsPath, QString createScript = QString(), QString command = QString());
	bool isVirtualEnvironment(QString path);
	QString getFixedEnvironmentSubdir();
	QString getFixedEnvironmentSubdirWindows();
	QString removeTrailingPythonVariable(QString environmentPath);
	bool showVenvInfoDialog(QString venvPath, QString createCommand);
	bool createVenv(QString createCommand, QString command);
	bool setScriptEngine(CommandStringVariables variables);
	bool initRaidionicsEngine(CommandStringVariables variables);
	void setOutputColorsFromClasses();
	void setContourFilteringFromClasses();
	int getClassNumber(QString filePath);
	ORGAN_TYPE getOrganType(int classNumber);
	QString colorForOrganType(QString outputClass);
	int contourFilterSettingForOrganType(QString outputClass);

	FilePathPropertyPtr mScriptFile;
	FilePreviewPropertyPtr mScriptFilePreview;
	OutputVariables mOutputVariables;

	vtkImageDataPtr mRawResult;
	QString mOutputChannelName;
	// mCommandLine is read from the main thread (requestStop(), and the
	// processXxx() slots invoked via queued connections) while it is
	// created/reset from the worker thread (createProcess()/deleteProcess(),
	// called from execute()). All access goes through
	// getCommandLine()/setCommandLine() so the shared_ptr's own read/write
	// is never racy; the ProcessWrapper it points to is not otherwise
	// protected, since only one thread ever owns it at a time.
	ProcessWrapperPtr mCommandLine;
	QMutex mCommandLineMutex;
	// Set by requestStop() (main thread), read by execute() (worker thread)
	// after the process exits, so a script that catches SIGTERM and exits
	// 0 is still treated as stopped rather than as a successful run.
	QAtomicInt mStopRequested;
	QString mResultFileEnding;
	QStringList mOutoutOrgans;
	ImagePtr mOutputImage;
	QList<QColor> mOutputColors;
	QStringList mOutputClasses;
	QStringList mOutputColorList;
	QList<int> mSmoothingSettings;

	SelectDataStringPropertyBasePtr mOutputImageSelectDataPtr;
	StringPropertySelectMeshPtr mOutputMeshSelectMeshPtr;
	BoolPropertyPtr mOutputMeshOption;
	SCRIPT_ENGINE mScriptEngine = seUnknown;
	RaidionicsPtr mRaidionicsUtilities = nullptr;
	QString mLineBuffer;
	QString mExtraCommandLineArguments;
	QMap<QString, QString> mExtraEnvironmentVariables;

protected slots:
	void scriptFileChanged();
	void processStateChanged();
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

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
//#include <QProcess>
#include "cxSettings.h"
#include "cxProcessWrapper.h"
#include <QColor>
#include "cxSelectDataStringProperty.h"


namespace cx
{

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
    bool mCreateOutputVolume;
    bool mCreateOutputMesh;
    QStringList mOutputColorList;
    QStringList mOutputClasses;

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
	bool isUsingDeepSintefEngine(CommandStringVariables variables);
	QString deepSintefCommandString(CommandStringVariables variables);
	
	bool environmentExist(QString path);
	QString getEnvironmentPath(CommandStringVariables variables);
	QString getEnvironmentBasePath(QString environmentPath);
	QString findRequirementsFileLocation(QString path);
	bool createVirtualPythonEnvironment(QString environmentPath, QString requirementsPath);
	bool isVirtualEnvironment(QString path);
	QString getFixedEnvironmentSubdir();

    FilePathPropertyPtr mScriptFile;
	FilePreviewPropertyPtr mScriptFilePreview;

	vtkImageDataPtr mRawResult;
	QString mOutputChannelName;
	QString mScriptPathAddition;
	ProcessWrapperPtr mCommandLine;
	QString mResultFileEnding;
	QStringList mOutoutOrgans;
	ImagePtr mOutputImage;
	QList<QColor> mOutputColors;
	QStringList mOutputClasses;

    SelectDataStringPropertyBasePtr mOutputImageSelectDataPtr;
    StringPropertySelectMeshPtr mOutputMeshSelectMeshPtr;
	BoolPropertyPtr mOutputMeshOption;

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

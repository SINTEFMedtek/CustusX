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


namespace cx
{

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
	FilePreviewPropertyPtr getIniFileOption(QDomElement root);
	PatientModelServicePtr mPatientModelService;

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
	QString createCommandString(ImagePtr input);
	bool runCommandStringAndWait(QString command);
	QString getCustomPath();
	bool readGeneratedSegmentationFile();
	QString getScriptPath();
	QString getInputFilePath(ImagePtr input);
	QString getOutputFilePath(ImagePtr input);

	FilePathPropertyPtr mScriptFile;
	FilePreviewPropertyPtr mScriptFilePreview;

	vtkImageDataPtr mRawResult;
	QString mOutputChannelName;
	QString mScriptPathAddition;
	//QProcess* mProcess;
	ProcessWrapperPtr mCommandLine;
	QString mResultFileEnding;

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

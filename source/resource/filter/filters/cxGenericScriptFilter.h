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
#include <QProcess>


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
	StringPropertyPtr setScriptOutput(QDomElement root);
	FilePreviewPropertyPtr getIniFileOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
	QString createCommandString(QString inputFile);
	void runCommandString(QString command);

	FilePathPropertyPtr mScriptFile;
	FilePreviewPropertyPtr mScriptFilePreview;

	vtkImageDataPtr mRawResult;
	QProcess* mProcess;
	QString mOutputChannelName;
	QString mScriptPathAddition;

protected slots:
	void scriptFileChanged();
	void processStateChanged(QProcess::ProcessState newState);
	void processFinished(int code, QProcess::ExitStatus status);
	void processError(QProcess::ProcessError error);
	void processReadyRead();
	void processReadyReadError();

};
typedef boost::shared_ptr<class GenericScriptFilter> GenericScriptFilterPtr;


} // namespace cx



#endif // CXGENERICSCRIPTFILTER_H

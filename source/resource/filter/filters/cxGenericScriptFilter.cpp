/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGenericScriptFilter.h"
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include "cxAlgorithmHelpers.h"
#include "cxSelectDataStringProperty.h"
//#include "cxDataLocations.h"
#include "cxPatientModelService.h"

#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"
#include "cxImage.h"

#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"
#include "cxFilePreviewProperty.h"
#include "cxFilePathProperty.h"
#include "cxProfile.h"
#include "cxLogger.h"

namespace cx
{

GenericScriptFilter::GenericScriptFilter(VisServicesPtr services) :
	FilterImpl(services),
	mOutputChannelName("ExternalScript"),
	mScriptPathAddition("/Filter-Scripts")
{
	//Copied QProsess example from ElastixExecuter
	mProcess = new QProcess(this);
	connect(mProcess, &QProcess::stateChanged, this, &GenericScriptFilter::processStateChanged);
	//connect(mProcess, &QProcess::errorOccurred, this, &GenericScriptFilter::processError); //Not working with old Qt. Using below line instead
	connect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));

	//Ubuntu 16.04 build machine cannot use this new Qt functionality
	//connect(mProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
	//				[=](int exitCode, QProcess::ExitStatus exitStatus){ this->GenericScriptFilter::processFinished(exitCode, exitStatus); });
	//Reverting to old style
	connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));

	//Show output from process
	connect(mProcess, &QProcess::readyReadStandardOutput, this, &GenericScriptFilter::processReadyRead);
	connect(mProcess, &QProcess::readyReadStandardError, this, &GenericScriptFilter::processReadyReadError);
}

GenericScriptFilter::~GenericScriptFilter()
{
	disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	mProcess->close();
}

void GenericScriptFilter::processStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
	{
		CX_LOG_DEBUG() << "GenericScriptFilter process running";
		//emit started(0);
	}
	if (newState == QProcess::NotRunning)
	{
		CX_LOG_DEBUG() << "GenericScriptFilter process finished running";
		//emit finished();
	}
	if (newState == QProcess::Starting)
	{
		CX_LOG_DEBUG() << "GenericScriptFilter process starting";
	}
}

void GenericScriptFilter::processFinished(int code, QProcess::ExitStatus status)
{
	if (status == QProcess::CrashExit)
		reportError("GenericScriptFilter process crashed");
}

void GenericScriptFilter::processError(QProcess::ProcessError error)
{
	QString msg;
	msg += "GenericScriptFilter process reported an error: ";

	switch (error)
	{
	case QProcess::FailedToStart:
		msg += "Failed to start";
		break;
	case QProcess::Crashed:
		msg += "Crashed";
		break;
	case QProcess::Timedout:
		msg += "Timed out";
		break;
	case QProcess::WriteError:
		msg += "Write Error";
		break;
	case QProcess::ReadError:
		msg += "Read Error";
		break;
	case QProcess::UnknownError:
		msg += "Unknown Error";
		break;
	default:
		msg += "Invalid error";
	}

	reportError(msg);
}

void GenericScriptFilter::processReadyRead()
{
	CX_LOG_CHANNEL_INFO(mOutputChannelName) << QString(mProcess->readAllStandardOutput());
}

void GenericScriptFilter::processReadyReadError()
{
	CX_LOG_CHANNEL_ERROR(mOutputChannelName) << QString(mProcess->readAllStandardError());
}

QString GenericScriptFilter::getName() const
{
	return "Script";
}

QString GenericScriptFilter::getType() const
{
	return "generic_script_filter";
}

QString GenericScriptFilter::getHelp() const
{
	return "<html>"
			"<h3>Script.</h3>"
			"<p>Support for calling external scripts from Custus"
			"<p>Uses parameter file... "
			"....</p>"
	        "</html>";
}

FilePathPropertyPtr GenericScriptFilter::getParameterFile(QDomElement root)
{
	QStringList paths;
	paths << profile()->getPath()+mScriptPathAddition;

	mScriptFile =  FilePathProperty::initialize("scriptSelector",
													"Select configuration file",
													"Select configuration file that specifies which script and parameters to use",
													"",//FilePath
													paths, //Catalog
													root);
	mScriptFile->setGroup("File");
	connect(mScriptFile.get(), &FilePathProperty::changed, this, &GenericScriptFilter::scriptFileChanged);
	return mScriptFile;
}

FilePreviewPropertyPtr GenericScriptFilter::getIniFileOption(QDomElement root)
{
	QStringList paths;
	paths << profile()->getPath()+mScriptPathAddition;

	mScriptFilePreview = FilePreviewProperty::initialize("filename", "Filename",
											"Select a ini file for running command line script",
											mScriptFile->getValue(),
											paths,
											root);

	mScriptFilePreview->setGroup("File");
	this->scriptFileChanged();//Initialize with data from mScriptFile variable
	return mScriptFilePreview;
}

void GenericScriptFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getParameterFile(mOptions));
	// mOptionsAdapters.push_back(this->getScriptContent(mOptions));
	mOptionsAdapters.push_back(this->getIniFileOption(mOptions));
	// mOptionsAdapters.push_back(this->setScriptOutput(mOptions));

}

void GenericScriptFilter::scriptFileChanged()
{
	mScriptFilePreview->setValue(mScriptFile->getValue());
}

QString GenericScriptFilter::createCommandString(QString inputFilePath)
{
//	QString parameterFile = mScriptFile->getValue();
//	CX_LOG_DEBUG() << "Parameter file: " << parameterFile;

	CX_LOG_DEBUG() << "Input file: " << inputFilePath;
	QString profilePath = profile()->getPath()+mScriptPathAddition;
	CX_LOG_DEBUG() << "Path to parameter file: " << profilePath;
	QString dataPath = mPatientModelService->getActivePatientFolder();
	CX_LOG_DEBUG() << "Path to Data: " << dataPath;


	// Parse .ini file, build command
	QString commandString = "no_command_yet";

	return commandString;
}

void GenericScriptFilter::runCommandString(QString command)
{
	CX_LOG_DEBUG() << "Command to run: " << command;

	mProcess->start(command);
}

void GenericScriptFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select image input");
	mInputTypes.push_back(temp);
}

void GenericScriptFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output smoothed image");
	mOutputTypes.push_back(temp);

}

bool GenericScriptFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	QString inputFilePath = input->getFilename();
	// get output also?
	if (!input)
		return false;

	// Parse .ini file, create command string to run
	QString command = this->createCommandString(inputFilePath);

	// Run command string on console
	this->runCommandString(command);

	return true; // Check for error?
}

bool GenericScriptFilter::postProcess()
{
	if (!mRawResult)
		return false;

	// More code here?

	return true;

}


} // namespace cx


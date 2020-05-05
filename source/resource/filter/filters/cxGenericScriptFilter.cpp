/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGenericScriptFilter.h"
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <QTimer>

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
	mScriptPathAddition("/Filter-Scripts"),
	mCommandLine(NULL)
{
}

GenericScriptFilter::~GenericScriptFilter()
{
}

void GenericScriptFilter::processStateChanged()
{
	if(!mCommandLine || !mCommandLine->getProcess())
	{
		CX_LOG_ERROR() << "GenericScriptFilter::processStateChanged: Process not existing!";
		return;
	}

	QProcess::ProcessState newState = mCommandLine->getProcess()->state();
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
	QProcess* process = mCommandLine->getProcess();
	CX_LOG_CHANNEL_INFO(mOutputChannelName) << QString(process->readAllStandardOutput());
}

void GenericScriptFilter::processReadyReadError()
{
	QProcess* process = mCommandLine->getProcess();
	CX_LOG_CHANNEL_ERROR(mOutputChannelName) << QString(process->readAllStandardError());
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

QString GenericScriptFilter::createCommandString(ImagePtr input)
{
	// Get paths
	QString parameterFile = mScriptFile->getValue();
	QString inputFilePath = input->getFilename();
	QString parameterFilePath = profile()->getPath()+mScriptPathAddition;
	parameterFilePath.append("/" + parameterFile);
	QString dataPath = mServices->patient()->getActivePatientFolder();
	dataPath.append("/" + inputFilePath);

	// Parse .ini file, build command
	QSettings settings(parameterFilePath, QSettings::IniFormat);
	settings.beginGroup("script");
	QString commandString = settings.value("path").toString();
	commandString.append(" " + dataPath);
	commandString.append(" " + settings.value("arguments").toString());
	settings.endGroup();

	return commandString;
}

void GenericScriptFilter::runCommandString(QString command)
{
	CX_LOG_DEBUG() << "Command to run: " << command;

	QString parameterFilePath = profile()->getPath()+mScriptPathAddition;
	CX_LOG_DEBUG() << "parameterFilePath: " << parameterFilePath;
	createProcess();

	mCommandLine->launch(command);//TODO: Split out arguments in QStringList?
	//mCommandLine->waitForStarted();


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
	// get output also?
	if (!input)
		return false;

	// Parse .ini file, create command string to run
	QString command = this->createCommandString(input);

	// Run command string on console
	this->runCommandString(command);

	//bool finished = mCommandLine->waitForFinished(30000);
	//if(!finished)
	//	CX_LOG_ERROR() << "GenericScriptFilter::execute: Running process failed";
	this->disconnectProcess();
	//QTimer::singleShot(3000, this, &GenericScriptFilter::deleteProcess);

	return true; // Check for error?
}


void GenericScriptFilter::createProcess()
{
	mCommandLine.reset();//delete
	CX_LOG_DEBUG() << "createProcess";
	if(mCommandLine)
	{
		CX_LOG_WARNING() << "Process already created";
		return;
	}
	mCommandLine = ProcessWrapperPtr(new cx::ProcessWrapper("ScriptFilter"));
	connect(mCommandLine.get(), &ProcessWrapper::stateChanged, this, &GenericScriptFilter::processStateChanged);
	//Show output from process
	connect(mCommandLine->getProcess(), &QProcess::readyReadStandardOutput, this, &GenericScriptFilter::processReadyRead);
	connect(mCommandLine->getProcess(), &QProcess::readyReadStandardError, this, &GenericScriptFilter::processReadyReadError);
}

void GenericScriptFilter::deleteProcess()
{
	CX_LOG_DEBUG() << "deleteProcess";
	if(mCommandLine)
	{
		CX_LOG_DEBUG() << "deleting";
		mCommandLine.reset();
	}
}

void GenericScriptFilter::disconnectProcess()
{
	CX_LOG_DEBUG() << "disconnectProcess";
	if(mCommandLine)
	{
		CX_LOG_DEBUG() << "disconnecting";
		disconnect(mCommandLine.get(), &ProcessWrapper::stateChanged, this, &GenericScriptFilter::processStateChanged);
		disconnect(mCommandLine->getProcess(), &QProcess::readyReadStandardOutput, this, &GenericScriptFilter::processReadyRead);
		disconnect(mCommandLine->getProcess(), &QProcess::readyReadStandardError, this, &GenericScriptFilter::processReadyReadError);
	}
}

bool GenericScriptFilter::postProcess()
{
	CX_LOG_DEBUG() << "postProcess";
	//disconnectProcess();
	//this->deleteProcess();
	//QTimer::singleShot(0, this, &GenericScriptFilter::deleteProcess);
	if (!mRawResult)
		return false;

	// More code here?
	return true;

}


} // namespace cx


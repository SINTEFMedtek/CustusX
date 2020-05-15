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
#include <QFileInfo>

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
#include "cxFileManagerService.h"
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
	mScriptPathAddition("/filter_scripts"),
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
		//Seems like this slot may get called after mCommandLine process is deleted
		//CX_LOG_ERROR() << "GenericScriptFilter::processStateChanged: Process not existing!";
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
	if(!mCommandLine || !mCommandLine->getProcess())
		return;

	QProcess* process = mCommandLine->getProcess();
	CX_LOG_CHANNEL_INFO(mOutputChannelName) << QString(process->readAllStandardOutput());
}

void GenericScriptFilter::processReadyReadError()
{
	if(!mCommandLine || !mCommandLine->getProcess())
		return;

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
	mOptionsAdapters.push_back(this->getIniFileOption(mOptions));

}

void GenericScriptFilter::scriptFileChanged()
{
	mScriptFilePreview->setValue(mScriptFile->getValue());
}

QString GenericScriptFilter::createCommandString(ImagePtr input)
{
	// Get paths
	QString parameterFile = mScriptFile->getValue();
	QString parameterFilePath = profile()->getPath()+mScriptPathAddition;
	parameterFilePath.append("/" + parameterFile);
	QString inputFilePath = getInputFilePath(input);
	QString outputFilePath = getOutputFilePath(input);
	CX_LOG_DEBUG() << "parameterFilePath: " << parameterFilePath;

	// Parse .ini file, build command
	QSettings settings(parameterFilePath, QSettings::IniFormat);
	settings.beginGroup("script");
	QString commandString = settings.value("path").toString();
	commandString.append(" " + inputFilePath);
	commandString.append(" " + outputFilePath);
	commandString.append(" " + settings.value("arguments").toString());
	settings.endGroup();

	return commandString;
}

QString GenericScriptFilter::getInputFilePath(ImagePtr input)
{
	QString inputFileName = input->getFilename();
	QString inputFilePath = mServices->patient()->getActivePatientFolder();
	inputFilePath.append("/" + inputFileName);
	return inputFilePath;
}

QString GenericScriptFilter::getOutputFilePath(ImagePtr input)
{
	QFileInfo fi(input->getFilename());
	QString outputFileName = fi.baseName();
	QString outputFilePath = mServices->patient()->getActivePatientFolder();
	CX_LOG_DEBUG() << "ActivePatientFolder (output): " << outputFilePath;
	QString parameterFile = mScriptFile->getValue();
	QString parameterFilePath = profile()->getPath()+mScriptPathAddition;
	parameterFilePath.append("/" + parameterFile);

	// Parse .ini file, get file_append
	QSettings settings(parameterFilePath, QSettings::IniFormat);
	settings.beginGroup("output");
	QString file_append = settings.value("file_append","_copy.mhd").toString();
	settings.endGroup();

	outputFileName.append(file_append);
	outputFilePath.append("/" + fi.path());
	outputFilePath.append("/" + outputFileName);
	CX_LOG_DEBUG() << "outputFilePath: " << outputFilePath;

	return outputFilePath;
}

bool GenericScriptFilter::runCommandStringAndWait(QString command)
{
	CX_LOG_DEBUG() << "Command to run: " << command;

	QString scriptWorkingDir = profile()->getPath()+mScriptPathAddition;
	CX_LOG_DEBUG() << "scriptWorkingDir: " << scriptWorkingDir;

	CX_ASSERT(mCommandLine);
	mCommandLine->getProcess()->setWorkingDirectory(scriptWorkingDir);
	bool success = mCommandLine->launch(command);
	if(success)
		return mCommandLine->waitForFinished();
	else
	{
		CX_LOG_WARNING() << "GenericScriptFilter::runCommandStringAndWait: Cannot start command!";
		return false;
	}
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
	if (!createProcess())
		return false;

	ImagePtr input = this->getCopiedInputImage();
	// get output also?
	if (!input)
		return false;

	// Parse .ini file, create command string to run
	QString command = this->createCommandString(input);

	//command = QString("echo test");//Test simple command

	// Run command string on console
	bool retval = this->runCommandStringAndWait(command);
	retval = retval & deleteProcess();

	return retval; // Check for error?
}


bool GenericScriptFilter::createProcess()
{
	mCommandLine.reset();//delete
	CX_LOG_DEBUG() << "createProcess";
	mCommandLine = ProcessWrapperPtr(new cx::ProcessWrapper("ScriptFilter"));
	//mCommandLine->turnOffReporting();//Seems like it's still not working. GenericScriptFilter::processReadyRead gets called after prosess is deleted?

	connect(mCommandLine.get(), &ProcessWrapper::stateChanged, this, &GenericScriptFilter::processStateChanged);
	//Show output from process
	//Looks like this may not work: ProcessReporter (used from ProcessWrapper) is already printing the output
	connect(mCommandLine->getProcess(), &QProcess::readyReadStandardOutput, this, &GenericScriptFilter::processReadyRead);
	connect(mCommandLine->getProcess(), &QProcess::readyReadStandardError, this, &GenericScriptFilter::processReadyReadError);
	return true;
}

bool GenericScriptFilter::deleteProcess()
{
	disconnectProcess();
	CX_LOG_DEBUG() << "deleteProcess";
	if(mCommandLine)
	{
		CX_LOG_DEBUG() << "deleting";
		mCommandLine.reset();
		return true;
	}
	return false;
}

bool GenericScriptFilter::disconnectProcess()
{
	CX_LOG_DEBUG() << "disconnectProcess";
	if(mCommandLine)
	{
		CX_LOG_DEBUG() << "disconnecting";
		disconnect(mCommandLine.get(), &ProcessWrapper::stateChanged, this, &GenericScriptFilter::processStateChanged);
		disconnect(mCommandLine->getProcess(), &QProcess::readyReadStandardOutput, this, &GenericScriptFilter::processReadyRead);
		disconnect(mCommandLine->getProcess(), &QProcess::readyReadStandardError, this, &GenericScriptFilter::processReadyReadError);
		return true;
	}
	return false;
}

bool GenericScriptFilter::postProcess()
{
	CX_LOG_DEBUG() << "postProcess";

	return readGeneratedSegmentationFile();

}

bool GenericScriptFilter::readGeneratedSegmentationFile()
{
	//TODO: Look at ElastixManager::addNonlinearData() for reading and adding new volume

	ImagePtr parentImage = this->getCopiedInputImage();
	if(!parentImage)
	{
		CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedSegmentationFile: No input image";
		return false;
	}
	QString uid = parentImage->getUid() + "_seg%1";
	QString imageName = parentImage->getName()+" seg%1";
	QString fileName = this->getOutputFilePath(parentImage);
	//QString fileName = getInputFilePath(parentImage);//TODO: Replace testcode with name of new file
	CX_LOG_DEBUG() << "Read new file: " << fileName;
	if (!QFileInfo(fileName).exists())
	{
		CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedSegmentationFile: Cannot find new file: " << fileName;
		return false;
	}

	ImagePtr newImage = boost::dynamic_pointer_cast<Image>(mServices->file()->load(uid, fileName));
	if(!newImage)
	{
		CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedSegmentationFile: No new image file created";
		return false;
	}
	if(!newImage->getBaseVtkImageData())
	{
		CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedSegmentationFile: New image file has no data";
		return false;
	}
	ImagePtr derivedImage = createDerivedImage(mServices->patient(),
										 uid, imageName,
										 newImage->getBaseVtkImageData(), parentImage);
	if(!derivedImage)
	{
		CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedSegmentationFile: Problem creating derived image";
		return false;
	}
	derivedImage->setImageType(istSEGMENTATION);//Mark with correct type

	mServices->patient()->insertData(derivedImage);

	// set output
	CX_ASSERT(mOutputTypes.size() > 0)
	mOutputTypes.front()->setValue(derivedImage->getUid());

	return true;
}

} // namespace cx


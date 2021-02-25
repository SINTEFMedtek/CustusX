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
#include <QDir>
#include <QDirIterator>

#include "cxAlgorithmHelpers.h"
#include "cxSelectDataStringProperty.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"

#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"
#include "cxImage.h"
#include "cxContourFilter.h"
#include "cxMesh.h"

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

CommandStringVariables::CommandStringVariables(QString parameterFilePath, ImagePtr input)
{
	// Parse .ini file
	QSettings settings(parameterFilePath, QSettings::IniFormat);
	settings.beginGroup("environment");
	envPath = settings.value("path").toString();
	settings.endGroup();
	settings.beginGroup("script");
	scriptFilePath = settings.value("path").toString();
	cArguments = settings.value("arguments").toString();
	scriptEngine = settings.value("engine").toString();
	model = settings.value("model").toString();
    settings.endGroup();
}

OutputVariables::OutputVariables(QString parameterFilePath)
{
    // Parse .ini file
    QSettings settings(parameterFilePath, QSettings::IniFormat);
    settings.beginGroup("output");
    mCreateOutputVolume = settings.value("volume").toBool();
    mCreateOutputMesh = settings.value("mesh").toBool();
    mMachineLearningOutput = settings.value("machine_learning").toBool();
    QString allColors = settings.value("color").toString();
    mOutputColorList = allColors.split(";");
    QString outputClass = settings.value("classes").toString();
    mOutputClasses = outputClass.split(" ");
    settings.endGroup();
}

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

void GenericScriptFilter::setParameterFilePath(QString path)
{
    mScriptFile->setValue(path);
    this->scriptFileChanged();
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
	CommandStringVariables variables = createCommandStringVariables(input);

	CX_LOG_DEBUG() << "deepSintefCommandString(variables): " << deepSintefCommandString(variables);

	if(isUsingDeepSintefEngine(variables))
		return deepSintefCommandString(variables);

	return standardCommandString(variables);
}

CommandStringVariables GenericScriptFilter::createCommandStringVariables(ImagePtr input)
{
	QString parameterFilePath = mScriptFile->getEmbeddedPath().getAbsoluteFilepath();
	CX_LOG_DEBUG() << "parameterFilePath: " << parameterFilePath;

	CommandStringVariables variables = CommandStringVariables(parameterFilePath, input);

	// Get paths
	variables.inputFilePath = getInputFilePath(input);
	variables.outputFilePath = getOutputFilePath(input);

	return variables;
}

QString GenericScriptFilter::standardCommandString(CommandStringVariables variables)
{
	QString commandString = variables.envPath;
	commandString.append(" " + variables.scriptFilePath);
	commandString.append(" " + variables.inputFilePath);
	commandString.append(" " + variables.outputFilePath);
	commandString.append(" " + variables.cArguments);

	return commandString;
}

bool GenericScriptFilter::isUsingDeepSintefEngine(CommandStringVariables variables)
{
	if(QString::compare(variables.scriptEngine, "DeepSintef", Qt::CaseInsensitive) == 0)
		return true;
	return false;
}

QString GenericScriptFilter::deepSintefCommandString(CommandStringVariables variables)
{
	QString commandString = variables.envPath;
	commandString.append(" " + variables.scriptFilePath);
	commandString.append(" --Task database --Arguments ");
	commandString.append("\"");
	commandString.append("InputVolume ");
	commandString.append(variables.inputFilePath);
	commandString.append(",OutputLabel ");
	commandString.append(variables.outputFilePath);
	commandString.append(",ModelsList ");
	commandString.append(variables.model);
	commandString.append("\"");
	return commandString;
}

QString GenericScriptFilter::getScriptPath()
{
	QString retval;

	QString parameterFilePath = mScriptFile->getEmbeddedPath().getAbsoluteFilepath();

	QSettings settings(parameterFilePath, QSettings::IniFormat);
	settings.beginGroup("script");
	QString scriptFilePath = settings.value("path").toString();//input instead?
	settings.endGroup();

	scriptFilePath.replace("./","/");

	retval = QFileInfo(parameterFilePath).absoluteDir().absolutePath()+QFileInfo(scriptFilePath).dir().path();
	CX_LOG_DEBUG() << "Pyton script file path: " << retval;

	retval = QFileInfo(parameterFilePath).absoluteDir().absolutePath();
	CX_LOG_DEBUG() << "Using ini file path as script path: " << retval;

	return retval;
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
	QString parameterFilePath = mScriptFile->getEmbeddedPath().getAbsoluteFilepath();

	// Parse .ini file, get file_append
	QSettings settings(parameterFilePath, QSettings::IniFormat);
	settings.beginGroup("output");
	mResultFileEnding = settings.value("file_append","_copy.mhd").toString();
	//mOutoutOrgans = settings.value("organs").toString().split(",");
	settings.endGroup();

	outputFileName.append(mResultFileEnding);
	outputFilePath.append("/" + fi.path());
	outputFilePath.append("/" + outputFileName);
	CX_LOG_DEBUG() << "outputFilePath: " << outputFilePath;

	return outputFilePath;
}

bool GenericScriptFilter::runCommandStringAndWait(QString command)
{
	CX_LOG_DEBUG() << "Command to run: " << command;

	QString parameterFilePath = mScriptFile->getEmbeddedPath().getAbsoluteFilepath();

	CX_ASSERT(mCommandLine);
	mCommandLine->getProcess()->setWorkingDirectory(getScriptPath()); //TODO: Use ini file path or python script file path?
	bool success = mCommandLine->launch(command);
	if(success)
        return mCommandLine->waitForFinished(1000*60*30);//Wait at least 30 min
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
    mOutputImageSelectDataPtr = StringPropertySelectData::New(mServices->patient());
    mOutputImageSelectDataPtr->setValueName("Output");
    mOutputImageSelectDataPtr->setHelp("Output smoothed image");
    mOutputTypes.push_back(mOutputImageSelectDataPtr);

    mOutputMeshSelectMeshPtr = StringPropertySelectMesh::New(mServices->patient());
    mOutputMeshSelectMeshPtr->setValueName("Output Mesh");
    mOutputMeshSelectMeshPtr->setHelp("Output surface model");
    mOutputTypes.push_back(mOutputMeshSelectMeshPtr);

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
	if(!retval)
		CX_LOG_WARNING() << "External process failed. QProcess::ProcessError: " << mCommandLine->getProcess()->error();
	retval = retval & deleteProcess();

	return retval; // Check for error?
}


bool GenericScriptFilter::createProcess()
{
	mCommandLine.reset();//delete
	CX_LOG_DEBUG() << "createProcess";
	mCommandLine = ProcessWrapperPtr(new cx::ProcessWrapper("ScriptFilter"));
	mCommandLine->turnOffReporting();//Handle output in this class instead

	// Merge channels to get all output in same channel in CustusX console
	mCommandLine->getProcess()->setProcessChannelMode(QProcess::MergedChannels);

	connect(mCommandLine.get(), &ProcessWrapper::stateChanged, this, &GenericScriptFilter::processStateChanged);

	/**************************************************************************
	* NB: For Python output to be written Python buffering must be turned off:
	* E.g. Use python -u
	**************************************************************************/
	//Show output from process
	connect(mCommandLine->getProcess(), &QProcess::readyRead, this, &GenericScriptFilter::processReadyRead);
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
		disconnect(mCommandLine->getProcess(), &QProcess::readyRead, this, &GenericScriptFilter::processReadyRead);
		return true;
	}
	return false;
}

bool GenericScriptFilter::postProcess()
{
	CX_LOG_DEBUG() << "postProcess";

    QString parameterFilePath = mScriptFile->getEmbeddedPath().getAbsoluteFilepath();
    OutputVariables outputVariables = OutputVariables(parameterFilePath);

    bool createOutputVolume = outputVariables.mCreateOutputVolume;
    bool createOutputMesh = outputVariables.mCreateOutputMesh;
    bool machineLearning = outputVariables.mMachineLearningOutput;
    QStringList colorList = outputVariables.mOutputColorList;
    mOutputClasses = outputVariables.mOutputClasses;
    this->setupOutputColors(colorList);

    return readGeneratedSegmentationFile(createOutputVolume, createOutputMesh, machineLearning);
}

void GenericScriptFilter::setupOutputColors(QStringList colorList)
{
    mOutputColors.clear();
    int i=0;
    do
    {
        if (colorList.size() > i)
        {
            QStringList color = colorList[i].split(",");
            QColor addColor = createColor(color);
            mOutputColors.append(addColor);
        }
        else
        {
            QString outputClass("");
            if(mOutputClasses.size() >= i)
              outputClass = mOutputClasses[i];
            CX_LOG_WARNING() << "In GenericScriptFilter::setupOutputColors(): No color set in ini for " << outputClass << " file. Setting mesh color to red.";
            mOutputColors.append(getDefaultColor());
        }
    }
    while (++i < mOutputClasses.size());
}

QColor GenericScriptFilter::createColor(QStringList color)
{
  QColor retval;
  if (color.size() == 4)
      retval.setRgb(color[0].toDouble(), color[1].toDouble(), color[2].toDouble(), color[3].toDouble());
  if (!retval.isValid())
  {
      CX_LOG_WARNING() << "In GenericScriptFilter::createColor(): Invalid color set in ini file. Setting color to red.";
      retval = getDefaultColor();
  }
  return retval;
}

QColor GenericScriptFilter::getDefaultColor()
{
  QColor retval;
  retval.setNamedColor("red");
  return retval;
}

void GenericScriptFilter::createOutputMesh(QColor color)
{

    // Make contour of segmented volume
	double threshold = 1; /// because the segmented image is 0..1

    vtkPolyDataPtr rawContour = ContourFilter::execute(
	mOutputImage->getBaseVtkImageData(),
	threshold,
	false, // reduce resolution
	true, // smoothing
    true, // keep topology
	0 // target decimation
	);

	QString uidOutputMesh = mOutputImage->getUid() + "_mesh";
	QString nameOutputMesh = mOutputImage->getName() + "_mesh";
	MeshPtr outputMesh = patientService()->createSpecificData<Mesh>(uidOutputMesh, nameOutputMesh);
	outputMesh->setVtkPolyData(rawContour);
	outputMesh->setColor(color);
	patientService()->insertData(outputMesh);
	outputMesh->get_rMd_History()->setRegistration(mOutputImage->get_rMd());
	mServices->view()->autoShowData(outputMesh);

    mOutputMeshSelectMeshPtr->setValue(outputMesh->getUid());
}

bool GenericScriptFilter::readGeneratedSegmentationFile(bool createOutputVolume, bool createOutputMesh, bool machineLearing)
{
	ImagePtr parentImage = this->getCopiedInputImage();
	if(!parentImage)
	{
		CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedSegmentationFile: No input image";
		return false;
	}
	QString nameEnding = mResultFileEnding;
	nameEnding.replace(".mhd", "");

    if (machineLearing)
        return this->readMachineLearningFiles(parentImage, nameEnding, createOutputVolume, createOutputMesh);
    else
        return this->readStandardFile(parentImage, nameEnding, createOutputVolume, createOutputMesh);
}

bool GenericScriptFilter::readStandardFile(ImagePtr parentImage, QString nameEnding, bool createOutputVolume, bool createOutputMesh)
{
    QString uid = parentImage->getUid() + nameEnding;
    QString imageName = parentImage->getName() + nameEnding;
    QString fileName = this->getOutputFilePath(parentImage);

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
    mOutputImage = createDerivedImage(mServices->patient(),
                                         uid, imageName,
                                         newImage->getBaseVtkImageData(), parentImage);
    if(!mOutputImage)
    {
        CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedSegmentationFile: Problem creating derived image";
        return false;
    }

    if (createOutputVolume)
        this->createOutputVolume();

    if(createOutputMesh && mOutputImage)
    {
        if (!mOutputColors.isEmpty())
            this->createOutputMesh(mOutputColors.at(0));
        else
        {
            CX_LOG_WARNING() << "In GenericScriptFilter::readGeneratedSegmentationFile(): No valid color set. Setting mesh color to red.";
            this->createOutputMesh(getDefaultColor());
        }
    }

    return true;
}

bool GenericScriptFilter::readMachineLearningFiles(ImagePtr parentImage, QString nameEnding, bool createOutputVolume, bool createOutputMesh)
{
    QFileInfo fileInfoInput(parentImage->getFilename());
    QString outputFileName = fileInfoInput.baseName();
	QFileInfo outputFileInfo(outputFileName.append(mResultFileEnding));
	QString outputFilePath = mServices->patient()->getActivePatientFolder();
    QString outputDir(outputFilePath.append("/" + fileInfoInput.path()));
	QString outputFileNamesNoExtention = outputFileInfo.baseName();

	QDirIterator fileIterator(outputDir, QDir::Files);
	while (fileIterator.hasNext())
	{
		QString filePath = fileIterator.next();
		if(filePath.contains(outputFileNamesNoExtention) && filePath.contains(".mhd"))
		{
            QFileInfo fileInfoOutput(filePath);
            QString uid =	fileInfoOutput.fileName().replace(".mhd", "");
			ImagePtr newImage = boost::dynamic_pointer_cast<Image>(mServices->file()->load(uid, filePath));
			if(!newImage)
			{
				CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedMachineLearningSegmentationFiles: No new image file created";
				continue;
			}

			mOutputImage = createDerivedImage(mServices->patient(),
												uid, uid,
												newImage->getBaseVtkImageData(), parentImage);
			if(!mOutputImage)
			{
				CX_LOG_WARNING() << "GenericScriptFilter::readGeneratedMachineLearningSegmentationFiles: Problem creating derived image";
				continue;
            }
			if (createOutputVolume)
                this->createOutputVolume();

            if(createOutputMesh && mOutputImage)
            {
                int colorNumber = 0;
                for(int i=0; i<mOutputClasses.size(); i++)
                {
                    if(filePath.contains(mOutputClasses[i], Qt::CaseSensitive))
                    {
                        colorNumber = i;
                        break;
                    }
                }
                this->createOutputMesh(mOutputColors.at(std::min(colorNumber, mOutputColors.size() - 1)));
            }
            this->deleteNotUsedFiles(filePath, createOutputVolume);
        }
	}

	return true;
}

void GenericScriptFilter::createOutputVolume()
{
    if(!mOutputImage)
        return;

    mOutputImage->setImageType(istSEGMENTATION);//Mark with correct type
    mOutputImage->resetTransferFunctions();//Reset transfer functions to get some useful values for visualization

    mServices->patient()->insertData(mOutputImage);
    mServices->view()->autoShowData(mOutputImage);

    mOutputImageSelectDataPtr->setValue(mOutputImage->getUid());
}

void GenericScriptFilter::deleteNotUsedFiles(QString fileNameMhd, bool createOutputVolume)
{
    //delete files not used anymore
    if (QFileInfo(fileNameMhd).exists() && !createOutputVolume)
        QFile(fileNameMhd).remove();

    QString fileNameRaw = fileNameMhd.left(fileNameMhd.lastIndexOf("."))+".raw";
    if (QFileInfo(fileNameRaw).exists() && !createOutputVolume)
        QFile(fileNameRaw).remove();

    QString fileNameNii = fileNameMhd.left(fileNameMhd.lastIndexOf("."))+".nii";
    if (QFileInfo(fileNameNii).exists())
        QFile(fileNameNii).remove();
}

} // namespace cx


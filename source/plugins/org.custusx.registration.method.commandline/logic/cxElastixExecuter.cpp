/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxElastixExecuter.h"

#include <QProcess>
#include <QFile>
#include <QDir>
#include <QtGlobal>
#include <QTextStream>

#include "cxTypeConversions.h"
#include "cxTime.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxBoundingBox3D.h"
#include "cxTransformFile.h"
#include "cxCustomMetaImage.h"

#include "cxPatientModelService.h"

namespace cx
{

ElastixExecuter::ElastixExecuter(RegServicesPtr services, QObject* parent) :
	TimedBaseAlgorithm("ElastiX", 1000),
	mServices(services)
{
	mProcess = new QProcess(this);
	connect(mProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(processStateChanged(QProcess::ProcessState)));
	connect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
}

ElastixExecuter::~ElastixExecuter()
{
	disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	mProcess->close();
}

void ElastixExecuter::setDisplayProcessMessages(bool on)
{
	disconnect(mProcess, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

	if (on)
	{
		connect(mProcess, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
		//	mProcess->setProcessChannelMode(QProcess::ForwardedChannels);
		mProcess->setProcessChannelMode(QProcess::MergedChannels);
		mProcess->setReadChannel(QProcess::StandardOutput);
	}
}


bool ElastixExecuter::setInput(QString application,
				DataPtr fixed,
				DataPtr moving,
				QString outdir,
				QStringList parameterfiles)
{
	mFixed = fixed;
	mMoving = moving;
	mOutdir = outdir;
	mParameterfiles = parameterfiles;
	mElastiXApplication = application;

	if (!fixed || !moving)
	{
		reportWarning("Failed to start elastiX registration, fixed or missing image missing.");
		return false;
	}

	if (mProcess->state() != QProcess::NotRunning)
	{
		reportWarning("Failed to start elastiX registration, process already running");
		return false;
	}

	// create output dir (required by ElastiX)
	QDir().mkpath(outdir);

	QString initFilename = this->writeInitTransformToElastixfile(fixed, moving, outdir);
	this->writeInitTransformToCalfile(fixed, moving, outdir);

	mLastOutdir = outdir;

	QStringList cmd;
	cmd << "\"" + application + "\"";
	cmd << "-f" << mServices->patient()->getActivePatientFolder()+"/"+fixed->getFilename();
	cmd << "-m" << mServices->patient()->getActivePatientFolder()+"/"+moving->getFilename();
	cmd << "-out" << outdir;
	cmd << "-t0" << initFilename;
	for (int i=0; i<parameterfiles.size(); ++i)
		cmd << "-p" << parameterfiles[i];

	QString commandLine = cmd.join(" ");
	report(QString("Executing registration with command line: [%1]").arg(commandLine));

	updateProcessName();
	this->setProcessEnvironment(mProcess);
	mProcess->start(commandLine);
	return true;
}

void ElastixExecuter::updateProcessName(bool transformix)
{
	if(transformix)
		mProduct = "Transformix";
	else
		mProduct = "ElastiX";
}

bool ElastixExecuter::runTransformix(ImagePtr deformImage)
{
	//Need a separete process for Transformix
	mTransformixProcess = new QProcess(this);
	connect(mTransformixProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(processStateChanged(QProcess::ProcessState)));
	connect(mTransformixProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	connect(mTransformixProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));

	mDeformImage = deformImage;
	QString commandLine = createTransformixCommandLine(mElastiXApplication, mOutdir, mParameterfiles);
	report(QString("Executing deformation with command line: [%1]").arg(commandLine));

	updateProcessName(true);
	this->setProcessEnvironment(mTransformixProcess);
	mTransformixProcess->start(commandLine);
	return true;
}

void ElastixExecuter::setProcessEnvironment(QProcess* process)
{
#ifdef Q_OS_LINUX
	// hack that inserts . into library path for linux. Solveds issue with elastix lib not being fixed up on linux.
	QString path = QFileInfo(mElastiXApplication).absolutePath();
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	env.insert("LD_LIBRARY_PATH", path);
	process->setProcessEnvironment(env);
#endif
}

QString ElastixExecuter::createTransformixCommandLine(QString elastixApplication, QString outdir, QStringList parameterfiles)
{
	if(!mDeformImage)
	{
		CX_LOG_DEBUG() << "ElastixExecuter::createTransformixCommandLine: No mDeformImage. No need to run transformix";
		return QString("");
	}
	//Assuming transformix is placed together with elastix
	QString path = QFileInfo(elastixApplication).absolutePath();
	QString transformixApplication = path + "/" + "transformix";
	if(!QFile::exists(transformixApplication))
	{
		CX_LOG_WARNING() << "ElastixExecuter::createTransformixCommandLine: Cannot find transformix application at: " << transformixApplication;
		return QString("");
	}

//	QString transformParametersFile = this->findMostRecentTransformOutputFile();
	int transformParameterFileNumber = parameterfiles.size() - 1;
	QString transformParametersFile = QString(mLastOutdir + "/TransformParameters.%1.txt").arg(transformParameterFileNumber);
	if (transformParametersFile.isEmpty())
	{
		CX_LOG_WARNING() << "ElastixExecuter::createTransformixCommandLine: Cannot find transformParameters";
		return QString("");
	}

	//This only seems to only do a clipping, and not run the transform
	//Turing this off requires removing parent from deformImage
//	QString initTransformFilename = this->writeInitTransformToElastixfile(mFixed, mDeformImage, mOutdir);
//	transformParametersFile = changeInitialTransformsFromParametersFile(transformParametersFile, initTransformFilename);//TODO: on/off? - Not always working?

	//Can only get this to work by moving mDeformImage back to start position (by undoing registration), and applying all Elastix transforms
	//This require skipping the 2 above lines, but handing either parent or moving the volume
	//Or remove mDeformImage parent before running Elastix (gives same starting position)
	//Skipping adding a CX registration from Elastix should work, or postponing it

	QStringList cmd;
	cmd << "\"" + transformixApplication + "\"";
	cmd << "-in" << mServices->patient()->getActivePatientFolder()+"/"+mDeformImage->getFilename();
	cmd << "-out" << outdir;
	cmd << "-tp" << transformParametersFile;

	QString commandLine = cmd.join(" ");
	return commandLine;
}

//Need to add an initial transform
//QString ElastixExecuter::changeInitialTransformsFromParametersFile(QString transformParametersFile, QString initTransformFilename)
//{
//	QString path = QFileInfo(transformParametersFile).absolutePath();
//	QString newFileName = path + "/" + "TransformParametersDeformationOnly.txt";

////	CX_LOG_DEBUG() << "ElastixExecuter::changeInitialTransformsFromParametersFile: convert " << transformParametersFile << " (read) to " << newFileName << " (write)";

//	QFile oldFile(transformParametersFile);
//	if(!oldFile.open(QIODevice::ReadOnly|QIODevice::Text))
//	{
//		CX_LOG_WARNING() << "ElastixExecuter::changeInitialTransformsFromParametersFile: Cannot open file " << transformParametersFile << " for reading";
//		return transformParametersFile;
//	}

//	QString fileContents;
//	QTextStream inStream(&oldFile);
//	QString fileLine;
//	while (inStream.readLineInto(&fileLine))
//	{
//		if(fileLine.startsWith("(InitialTransformParametersFileName"))
//		{
//			if(initTransformFilename.isEmpty())
//				fileLine = "(InitialTransformParametersFileName \"NoInitialTransform\")";//Not enough
//			fileLine = "(InitialTransformParametersFileName \"" + initTransformFilename + "\")";
//		}
//		fileContents += fileLine + "\n";
//	}
//	oldFile.close();

//	QFile newFile(newFileName);
//	if(!newFile.open(QIODevice::WriteOnly|QIODevice::Text))
//	{
//		CX_LOG_WARNING() << "ElastixExecuter::changeInitialTransformsFromParametersFile: Cannot open file " << newFileName << " for writeing";
//		return transformParametersFile;
//	}

//	QTextStream outStream(&newFile);
//	outStream << fileContents;
//	newFile.close();

//	return newFileName;
//}

void ElastixExecuter::execute()
{
	emit aboutToStart(); // if properly set: triggers the setInput()
}

bool ElastixExecuter::isFinished() const
{
	std::cout << "TODO: check ElastixExecuter::isFinished()" << std::endl;
	return mProcess->atEnd();
}

bool ElastixExecuter::isRunning() const
{
	bool retval = mProcess->state()!=QProcess::NotRunning;
	if(mTransformixProcess)
		retval = retval || mTransformixProcess->state()!=QProcess::NotRunning;
	return retval;
}

QString ElastixExecuter::writeInitTransformToElastixfile(
	DataPtr fixed,
	DataPtr moving,
	QString outdir)
{
	// elastiX uses the transforms present in the mhd files. If the mhd info is up to date
	// with the dataManager info, the T0=I, i.e we dont need to tell elastiX anything.
	// If NOT up to date, then compare file and dataManager, then insert the difference as T0:
	//
	// Let f be fixed, m be moving, mm and ff be the intermediate spaces between the file and r:
	//  	rMd is the transform stored in ssc.
	//		ddMd is the transform stored in the mhd file.
	//		T0 is the remainder to be sent to elastiX
	//
	// 		mMf = mMr*rMf = mMmm*mmMr*rMff*ffMf = mMmm*T0*ffMf
	//               -->
	// 		T0 = mmMm*mMr*rMf*fMff
	//
	Transform3D rMf = fixed->get_rMd();
	Transform3D rMm = moving->get_rMd();
	Transform3D ffMf = this->getFileTransform_ddMd(fixed);
	Transform3D mmMm = this->getFileTransform_ddMd(moving);
	Transform3D mCenterMfCenter_r = centerToCenterTranslation(true);

//	Transform3D mMf = rMm.inv() * rMf;
	// -->
	// The remainder transform, not stored in mhd files, must be sent to elastiX:
	Transform3D T0 = mmMm*rMm.inv()*mCenterMfCenter_r*rMf*ffMf.inv();

//	Transform3D mMf = moving->get_rMd().inv() * fixed->get_rMd();
	ElastixEulerTransform E = ElastixEulerTransform::create(T0, fixed->boundingBox().center());

	QString elastiXText = QString(""
		"// Input transform file\n"
		"// Auto-generated by CustusX on %1\n"
		"\n"
		"(Transform \"EulerTransform\")\n"
		"(NumberOfParameters 6)\n"
		"(TransformParameters %2 %3)\n"
		"(InitialTransformParametersFileName \"NoInitialTransform\")\n"
		"(HowToCombineTransforms \"Compose\")\n"
		"\n"
		"// EulerTransform specific\n"
		"(CenterOfRotationPoint %4)\n"
		"(ComputeZYX \"false\")\n"
		"").arg(QDateTime::currentDateTime().toString(timestampSecondsFormatNice()),
			qstring_cast(E.mAngles_xyz),
			qstring_cast(E.mTranslation),
			qstring_cast(E.mCenterOfRotation));

	QFile initTransformFile(outdir+"/t0.txt");
	if (!initTransformFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		reportWarning(QString("Failed to open file %1 for writing.").arg(initTransformFile.fileName()));
	}
	initTransformFile.write(elastiXText.toLatin1());
	return initTransformFile.fileName();
}

QString ElastixExecuter::writeInitTransformToCalfile(
	DataPtr fixed,
	DataPtr moving,
	QString outdir)
{
	Transform3D mCenterMfCenter_r = centerToCenterTranslation();
	Transform3D mMf = moving->get_rMd().inv() * mCenterMfCenter_r * fixed->get_rMd();

	TransformFile file(outdir+"/moving_M_fixed_initial.cal");
	file.write(mMf);

	return file.fileName();
}

Transform3D ElastixExecuter::centerToCenterTranslation(bool printDebug)
{
	if(volumesOverlap())
	{
		if(printDebug)
			CX_LOG_DEBUG() << "ElastixExecuter::centerToCenterTranslation: Volumes overlap. No need to move";
		return Transform3D::Identity();
	}

	Vector3D fCenter_r = mFixed->get_rMd().coord(mFixed->boundingBox().center());
	Vector3D mCenter_r = mMoving->get_rMd().coord(mMoving->boundingBox().center());
	Transform3D mCenterMfCenter_r = createTransformTranslate(mCenter_r - fCenter_r);//center to center translation in r
	if(printDebug)
		CX_LOG_DEBUG() << "Volumes sent to Elastix don't overlap. Apply transform to move center to center:\n" << mCenterMfCenter_r;
	return mCenterMfCenter_r;
}

bool ElastixExecuter::volumesOverlap()
{
	DoubleBoundingBox3D fixedBB = mFixed->boundingBox();
	DoubleBoundingBox3D movingBB = mMoving->boundingBox();
	fixedBB.translate(mFixed->get_rMd());
	movingBB.translate(mMoving->get_rMd());

	DoubleBoundingBox3D bbIntersection = intersection(fixedBB, movingBB);
//	CX_LOG_DEBUG() << "range fixedBB: " << fixedBB.range();
//	CX_LOG_DEBUG() << "range movingBB: " << movingBB.range();
//	CX_LOG_DEBUG() << "range bbIntersection: " << bbIntersection.range();
	if(bbIntersection==DoubleBoundingBox3D::zero())
		return false;

	//Require range above a threshold. Use 10% of range of fixed?
	Vector3D rangeFixed = fixedBB.range();
	double threshold = std::min(rangeFixed[0]/10.0, rangeFixed[2]/10.0);//Use x and z for now as x and y is usually equal
	Vector3D rangeIntersection = bbIntersection.range();
	double minIntersectionRange = std::min(rangeIntersection[0], rangeIntersection[2]);
//	CX_LOG_DEBUG() << "threshold: " << threshold;
//	CX_LOG_DEBUG() << "minIntersectionRange: " << minIntersectionRange;
	if(minIntersectionRange < threshold)
		return false;

	return true;
}

void ElastixExecuter::processReadyRead()
{
	report(QString(mProcess->readAllStandardOutput()));
}

void ElastixExecuter::processError(QProcess::ProcessError error)
{
	QString msg;
	msg += "Registration process reported an error: ";

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

void ElastixExecuter::processFinished(int code, QProcess::ExitStatus status)
{
	if (status == QProcess::CrashExit)
		reportError("Registration process crashed");
}


void ElastixExecuter::processStateChanged(QProcess::ProcessState newState)
{
// removed text: to much information

	QString msg;
	msg += "Registration process";

	if (newState == QProcess::Running)
	{
//		report(msg + " running.");
		emit started(0);
	}
	if (newState == QProcess::NotRunning)
	{
		emit finished();
//		report(msg + " not running.");
	}
	if (newState == QProcess::Starting)
	{
//		report(msg + " starting.");
	}
}

QString ElastixExecuter::findMostRecentTransformOutputFile() const
{
	QString retval;
	for (int i=0; ; ++i)
	{
		QString filename = QString(mLastOutdir + "/TransformParameters.%1.txt").arg(i);
		if (!QFileInfo::exists(filename))
			break;
		retval = filename;
	}
	return retval;
}

Transform3D ElastixExecuter::getFileTransform_ddMd(DataPtr volume)
{
	QString patFolder = mServices->patient()->getActivePatientFolder();
	CustomMetaImagePtr reader = CustomMetaImage::create(patFolder+"/"+volume->getFilename());
	Transform3D ddMd = reader->readTransform();
	return ddMd;
}

Transform3D ElastixExecuter::getAffineResult_mMf(bool* ok)
{
	Transform3D mmMff = this->getAffineResult_mmMff(ok);
	Transform3D ffMf = this->getFileTransform_ddMd(mFixed);
	Transform3D mmMm = this->getFileTransform_ddMd(mMoving);

	return mmMm.inv() * mmMff * ffMf;
}

Transform3D ElastixExecuter::getAffineResult_mmMff(bool* ok)
{
	QString filename = this->findMostRecentTransformOutputFile();
	Transform3D mMf = Transform3D::Identity();

	if (filename.isEmpty())
	{
		if (ok)
			*ok = false;

		TransformFile file(mLastOutdir+"/moving_M_fixed_registered.cal");
		mMf = file.read(ok);

		if (ok && !*ok)
		{
			reportWarning("Failed to read registration results.");
		}

		return mMf;
	}

	while (true)
	{
		ElastixParameterFile file(filename);

		bool useDirectionCosines = file.readParameterBool("UseDirectionCosines");
		if (useDirectionCosines)
		{
//			reportWarning("Elastix UseDirectionCosines is not supported. Result is probably wrong.");
		}

		QString transformType = file.readParameterString("Transform");
		if (transformType=="EulerTransform")
		{
			if (ok)
				*ok = true;
			Transform3D mQf = file.readEulerTransform();
			// concatenate transforms:
			mMf = mQf * mMf;
		}
		else if (transformType=="AffineTransform")
		{
			if (ok)
				*ok = true;
			Transform3D mQf = file.readAffineTransform();
			// concatenate transforms:
			mMf = mQf * mMf;
		}
		else
		{
			// accept invalid transforms, but emit warning.
//			if (ok)
//				*ok = false;
			reportWarning(QString("TransformType [%1] is not supported by CustusX. Registration result from %2 ignored.").arg(transformType).arg(filename));
		}

		filename = file.readParameterString("InitialTransformParametersFileName");
		if (filename.isEmpty() || filename=="NoInitialTransform")
			break;
	}

	return mMf;
}


QString ElastixExecuter::getNonlinearResultVolume(bool* ok, bool getTransformixResult)
{
	if (ok)
		*ok = false;

	QString retval;
	int i=0;
	for (; ; ++i)
	{
		//TODO only mhd supported
		QString filename = QString(mLastOutdir + "/result.%1.mhd").arg(i);
		if (!QFileInfo::exists(filename))
			break;
		retval = filename;
	}
	if(getTransformixResult)
	{
		QString filename = QString(mLastOutdir + "/result.mhd");
		if (QFileInfo::exists(filename))
			retval = filename;
	}

	if (retval.isEmpty())
		return retval;

	QString paramFilename = QString(mLastOutdir + "/TransformParameters.%1.txt").arg(i-1);
	ElastixParameterFile file(paramFilename);
	QString transform = file.readParameterString("Transform");

	if ((transform=="BSplineTransform") || (transform=="SplineKernelTransform"))
	{
		report(QString("Reading result file %1 created with transform %2").arg(retval).arg(transform));
		if (ok)
			*ok = true;
		return retval;
	}
	else
	{
		return "";
	}

}


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------




Transform3D ElastixParameterFile::readEulerTransform()
{
	QString transformType = this->readParameterString("Transform");
	if (transformType!="EulerTransform")
		reportError("Assert failure: attempting to read EulerTransform");

	int numberOfParameters = this->readParameterInt("NumberOfParameters");
	if (numberOfParameters!=6)
	{
		reportWarning(QString("Expected 6 Euler parameters, got %1").arg(numberOfParameters));
		return Transform3D::Identity();
	}
	std::vector<double> tp = this->readParameterDoubleVector("TransformParameters");
	std::vector<double> cor = this->readParameterDoubleVector("CenterOfRotationPoint");

	ElastixEulerTransform E = ElastixEulerTransform::create(
		Vector3D(tp[0], tp[1], tp[2]),
		Vector3D(tp[3], tp[4], tp[5]),
		Vector3D(cor[0], cor[1], cor[2]));

	return E.toMatrix();
}

Transform3D ElastixParameterFile::readAffineTransform()
{
	QString transformType = this->readParameterString("Transform");
	if (transformType!="AffineTransform")
		reportError("Assert failure: attempting to read AffineTransform");

	int numberOfParameters = this->readParameterInt("NumberOfParameters");
	if (numberOfParameters!=12)
	{
		reportWarning(QString("Expected 12 Euler parameters, got %1").arg(numberOfParameters));
		return Transform3D::Identity();
	}
	std::vector<double> tp = this->readParameterDoubleVector("TransformParameters");
//	std::vector<double> cor = this->readParameterDoubleVector("CenterOfRotationPoint");

	Transform3D M = Transform3D::Identity();

	for (int r=0; r<3; ++r)
		for (int c=0; c<3; ++c)
			M(r,c) = tp[3*r+c];
	for (int r=0; r<3; ++r)
		M(r,3) = tp[9+r];

	return M;
}

ElastixParameterFile::ElastixParameterFile(QString filename) : mFile(filename)
{
	if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		reportWarning(QString("Can't open ElastiX result file %1").arg(filename));
	}
	mText = QString(mFile.readAll());
//	std::cout << "Loaded text from " << filename << ":\n" << mText << std::endl;
}

QString ElastixParameterFile::readParameterRawValue(QString key)
{
	QStringList lines = mText.split('\n');

	QString regexpStr = QString(""
		"\\s*" // match zero or more whitespace
		"\\("  // match beginning (
		"\\s*" // match zero or more whitespace
		"%1"   // key
		"\\s+" // match one or more whitespace
		"("    // start grab value
		"[^\\)]*" // value - anything up to the closing )
		")"    // end grab value
		"\\)"  // match ending )
		"[^\\n]*" // rest of line - ignore
		).arg(key);
	QRegExp rx(regexpStr);
//	std::cout << regexpStr << std::endl;

	lines.indexOf(rx);

//	if (lines.indexOf(rx)>=0)
//	{
//		std::cout << "FOUND0 " << rx.cap(0) << std::endl;
//		std::cout << "FOUND1 " << rx.cap(1) << std::endl;
//	}

	return rx.cap(1);
}

QString ElastixParameterFile::readParameterString(QString key)
{
	QString retval =  this->readParameterRawValue(key);
	if (retval.startsWith("\"") && retval.endsWith("\""))
	{
		retval = retval.replace(0, 1, "");
		retval = retval.replace(retval.size()-1, 1, "");
	}

	return retval;
}

bool ElastixParameterFile::readParameterBool(QString key)
{
	QString text = this->readParameterString(key);
	return (text=="true") ? true : false;
}

int ElastixParameterFile::readParameterInt(QString key)
{
	QString retval =  this->readParameterRawValue(key);
	return retval.toInt();
}

std::vector<double> ElastixParameterFile::readParameterDoubleVector(QString key)
{
	QString retval =  this->readParameterRawValue(key);
	return convertQString2DoubleVector(retval);
}

} /* namespace cx */

// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.


#include "cxElastixExecuter.h"

#include <QProcess>
#include <QFile>
#include <QDir>
#include "sscMessageManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "sscTypeConversions.h"

namespace cx
{

ElastixExecuter::ElastixExecuter(QObject* parent) : TimedBaseAlgorithm("ElastiX", 1000)
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


void ElastixExecuter::run(QString application,
				ssc::ImagePtr fixed,
				ssc::ImagePtr moving,
				QString outdir,
				QStringList parameterfiles)
{
	if (!fixed || !moving)
	{
		ssc::messageManager()->sendWarning("Failed to start elastiX registration, fixed or missing image missing.");
		return;
	}

	if (mProcess->state() != QProcess::NotRunning)
	{
		ssc::messageManager()->sendWarning("Failed to start elastiX registration, process already running");
		return;
	}

	QFile initTransformFile(outdir+"/t0.txt");

	mLastOutdir = outdir;

	QStringList cmd;
	cmd << application;
	cmd << "-f" << patientService()->getPatientData()->getActivePatientFolder()+"/"+fixed->getFilePath();
	cmd << "-m" << patientService()->getPatientData()->getActivePatientFolder()+"/"+moving->getFilePath();
	cmd << "-out" << outdir;
//	cmd << "-t0" << initTransformFile.fileName();
	for (int i=0; i<parameterfiles.size(); ++i)
		cmd << "-p" << parameterfiles[i];

	QString commandLine = cmd.join(" ");
	ssc::messageManager()->sendInfo(QString("Executing registration with command line: [%1]").arg(commandLine));

	// create output dir (required by ElastiX)
	QDir().mkpath(outdir);

	mProcess->start(commandLine);
//	mProcess->start("elastix");
}


void ElastixExecuter::processReadyRead()
{
	ssc::messageManager()->sendInfo(QString(mProcess->readAllStandardOutput()));
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

	ssc::messageManager()->sendError(msg);
}

void ElastixExecuter::processFinished(int code, QProcess::ExitStatus status)
{
	ssc::messageManager()->sendInfo(QString("Registration process exited with code %1 and status %2").arg(code).arg(status));

//	this->getAffineResult();
}


void ElastixExecuter::processStateChanged(QProcess::ProcessState newState)
{
	QString msg;
	msg += "Registration process";

	if (newState == QProcess::Running)
	{
		ssc::messageManager()->sendInfo(msg + " running.");
		emit started(0);
//		this->delayedAutoConnectServer();
	}
	if (newState == QProcess::NotRunning)
	{
		emit finished();
		ssc::messageManager()->sendInfo(msg + " not running.");
	}
	if (newState == QProcess::Starting)
	{
		ssc::messageManager()->sendInfo(msg + " starting.");
	}

//
//	std::cout << "==============" << std::endl;
//	std::cout << QString(mProcess->readAllStandardError()) << std::endl;
//	std::cout << QString(mProcess->readAllStandardOutput()) << std::endl;
//	std::cout << "==============" << std::endl;
}


/** Return the result of the latest registration as a linear transform.
 *
 *  Important: The result is according to the ElastiX spec:
 *  \verbatim
   In elastix the transformation is defined as a coordinate mapping from
   the fixed image domain to the moving image domain.
 *  \endverbatim
 *
 *
 */
ssc::Transform3D ElastixExecuter::getAffineResult_mMf(bool* ok)
{
	QString filename = mLastOutdir + "/TransformParameters.0.txt";
	ElastixParameterFile file(filename);

	bool useDirectionCosines = file.readParameterBool("UseDirectionCosines");
	if (useDirectionCosines)
	{
		ssc::messageManager()->sendWarning("Elastix UseDirectionCosines is not supported. Result is probably wrong.");
	}

	QString transformType = file.readParameterString("Transform");
	if (transformType=="EulerTransform")
	{
		if (ok)
			*ok = true;
		return file.readEulerTransform();
	}

	if (ok)
		*ok = false;
	ssc::messageManager()->sendWarning(QString("TransformType [%1] is not supported by CustusX. Registration result ignored.").arg(transformType));
	return ssc::Transform3D::Identity();
}





// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------




ssc::Transform3D ElastixParameterFile::readEulerTransform()
{
	QString transformType = this->readParameterString("Transform");
	if (transformType!="EulerTransform")
		ssc::messageManager()->sendError("Assert failure: attempting to read EulerTransform");

	int numberOfParameters = this->readParameterInt("NumberOfParameters");
	if (numberOfParameters!=6)
	{
		ssc::messageManager()->sendWarning(QString("Expected 6 Euler parameters, got %1").arg(numberOfParameters));
		return ssc::Transform3D::Identity();
	}
	std::vector<double> transformParameters = this->readParameterDoubleVector("TransformParameters");
	std::vector<double> centerOfRotationPoint = this->readParameterDoubleVector("CenterOfRotationPoint");

	// TODO add rotation as well
	ssc::Vector3D t(transformParameters[3], transformParameters[4], transformParameters[5]);
	return ssc::createTransformTranslate(t).inv();
}

ElastixParameterFile::ElastixParameterFile(QString filename) : mFile(filename)
{
	if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		ssc::messageManager()->sendWarning(QString("Can't open ElastiX result file %1").arg(filename));
	}
	mText = QString(mFile.readAll());
	std::cout << "Loaded text from " << filename << ":\n" << mText << std::endl;
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

	if (lines.indexOf(rx)>=0)
	{
		std::cout << "FOUND0 " << rx.cap(0) << std::endl;
		std::cout << "FOUND1 " << rx.cap(1) << std::endl;
	}

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

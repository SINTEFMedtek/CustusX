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

ssc::Transform3D ElastixExecuter::getAffineResult() const
{
	return ssc::Transform3D::Identity();
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

} /* namespace cx */

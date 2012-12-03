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
#include "sscTime.h"
#include "sscData.h"
#include "sscBoundingBox3D.h"
#include "cxTransformFile.h"
#include "sscCustomMetaImage.h"

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


void ElastixExecuter::setInput(QString application,
				ssc::DataPtr fixed,
				ssc::DataPtr moving,
				QString outdir,
				QStringList parameterfiles)
{
	mFixed = fixed;
	mMoving = moving;

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

	// create output dir (required by ElastiX)
	QDir().mkpath(outdir);

	QString initFilename = this->writeInitTransformToElastixfile(fixed, moving, outdir);
	this->writeInitTransformToCalfile(fixed, moving, outdir);

	mLastOutdir = outdir;

	QStringList cmd;
	cmd << application;
	cmd << "-f" << patientService()->getPatientData()->getActivePatientFolder()+"/"+fixed->getFilePath();
	cmd << "-m" << patientService()->getPatientData()->getActivePatientFolder()+"/"+moving->getFilePath();
	cmd << "-out" << outdir;
	cmd << "-t0" << initFilename;
	for (int i=0; i<parameterfiles.size(); ++i)
		cmd << "-p" << parameterfiles[i];

	QString commandLine = cmd.join(" ");
	ssc::messageManager()->sendInfo(QString("Executing registration with command line: [%1]").arg(commandLine));

	mProcess->start(commandLine);
//	mProcess->start("elastix");
}

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
    return !mProcess->state()!=QProcess::NotRunning;
}

/**Write the initial (pre-registration) mMf transform to
 * disk as required by elastix.
 */
QString ElastixExecuter::writeInitTransformToElastixfile(
	ssc::DataPtr fixed,
	ssc::DataPtr moving,
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
	ssc::Transform3D rMf = fixed->get_rMd();
	ssc::Transform3D rMm = moving->get_rMd();
	ssc::Transform3D ffMf = this->getFileTransform_ddMd(mFixed);
	ssc::Transform3D mmMm = this->getFileTransform_ddMd(mMoving);
//	ssc::Transform3D mMf = rMm.inv() * rMf;
	// -->
	// The remainder transform, not stored in mhd files, must be sent to elastiX:
	ssc::Transform3D T0 = mmMm*rMm.inv()*rMf*ffMf.inv();

//	ssc::Transform3D mMf = moving->get_rMd().inv() * fixed->get_rMd();
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
		"").arg(QDateTime::currentDateTime().toString(ssc::timestampSecondsFormatNice()),
			qstring_cast(E.mAngles_xyz),
			qstring_cast(E.mTranslation),
			qstring_cast(E.mCenterOfRotation));

	QFile initTransformFile(outdir+"/t0.txt");
	if (!initTransformFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		ssc::messageManager()->sendWarning(QString("Failed to open file %1 for writing.").arg(initTransformFile.fileName()));
	}
	initTransformFile.write(elastiXText.toAscii());
	return initTransformFile.fileName();
}

/**Write the initial (pre-registration) mMf transform to
 * disk in a .cal file that contains only the 16 matrix numbers.
 */
QString ElastixExecuter::writeInitTransformToCalfile(
	ssc::DataPtr fixed,
	ssc::DataPtr moving,
	QString outdir)
{
	ssc::Transform3D mMf = moving->get_rMd().inv() * fixed->get_rMd();

	TransformFile file(outdir+"/moving_M_fixed_initial.cal");
	file.write(mMf);

	return file.fileName();
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
	if (status == QProcess::CrashExit)
		ssc::messageManager()->sendError("Registration process crashed");
}


void ElastixExecuter::processStateChanged(QProcess::ProcessState newState)
{
// removed text: to much information

	QString msg;
	msg += "Registration process";

	if (newState == QProcess::Running)
	{
//		ssc::messageManager()->sendInfo(msg + " running.");
		emit started(0);
	}
	if (newState == QProcess::NotRunning)
	{
		emit finished();
//		ssc::messageManager()->sendInfo(msg + " not running.");
	}
	if (newState == QProcess::Starting)
	{
//		ssc::messageManager()->sendInfo(msg + " starting.");
	}
}


/**Find the TransformParameters.i.txt file with the
 * highest i. All other transform files can be found from
 * this one.
 */
QString ElastixExecuter::findMostRecentTransformOutputFile() const
{
	QString retval;
	for (int i=0; ; ++i)
	{
		QString filename = QString(mLastOutdir + "/TransformParameters.%1.txt").arg(i);
		if (!QFileInfo(filename).exists())
			break;
		retval = filename;
	}
	return retval;
}

/**Return the transform present within the mhd file pointed to by the
 * input volume.
 *
 * This is part of the normal rMd transform within ssc::Data, but required
 * because elastiX reads and uses it.
 */
ssc::Transform3D ElastixExecuter::getFileTransform_ddMd(ssc::DataPtr volume)
{
	QString patFolder = patientService()->getPatientData()->getActivePatientFolder();
	ssc::CustomMetaImagePtr reader = ssc::CustomMetaImage::create(patFolder+"/"+volume->getFilePath());
	ssc::Transform3D ddMd = reader->readTransform();
	return ddMd;
}

/**Return the result of the latest registration as a linear transform mMf.
 *
 * Read the descriptions in writeInitTransformToElastixfile() and
 * getAffineResult_mmMff for a full discussion.
 *
 */
ssc::Transform3D ElastixExecuter::getAffineResult_mMf(bool* ok)
{
	ssc::Transform3D mmMff = this->getAffineResult_mmMff(ok);
	ssc::Transform3D ffMf = this->getFileTransform_ddMd(mFixed);
	ssc::Transform3D mmMm = this->getFileTransform_ddMd(mMoving);

	return mmMm.inv() * mmMff * ffMf;
}

/** Return the result of the latest registration as a linear transform mMf.
 *
 *  Important: The result is according to the ElastiX spec:
 *  \verbatim
   In elastix the transformation is defined as a coordinate mapping from
   the fixed image domain to the moving image domain.
 *  \endverbatim
 *
 * All transform files are read and concatenated. Those with
 * unrecognized (i.e. by CustusX) transforms are ignored with
 * a warning.
 *
 * NOTE: This 'inner' function returns the raw result from elastiX,
 * but CustusX expects that the file transforms of the fixed and moving
 * images are also contained in the result. Use the getAffineResult_mMf()
 * for the full result.
 *
 */
ssc::Transform3D ElastixExecuter::getAffineResult_mmMff(bool* ok)
{
	QString filename = this->findMostRecentTransformOutputFile();
	ssc::Transform3D mMf = ssc::Transform3D::Identity();

	if (filename.isEmpty())
	{
		if (ok)
			*ok = false;

		TransformFile file(mLastOutdir+"/moving_M_fixed_registered.cal");
		mMf = file.read(ok);

		if (ok && !*ok)
		{
			ssc::messageManager()->sendWarning("Failed to read registration results.");
		}

		return mMf;
	}

	while (true)
	{
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
			ssc::Transform3D mQf = file.readEulerTransform();
			// concatenate transforms:
			mMf = mQf * mMf;
		}
		else if (transformType=="AffineTransform")
		{
			if (ok)
				*ok = true;
			ssc::Transform3D mQf = file.readAffineTransform();
			// concatenate transforms:
			mMf = mQf * mMf;
		}
		else
		{
			// accept invalid transforms, but emit warning.
//			if (ok)
//				*ok = false;
			ssc::messageManager()->sendWarning(QString("TransformType [%1] is not supported by CustusX. Registration result from %2 ignored.").arg(transformType).arg(filename));
		}

		filename = file.readParameterString("InitialTransformParametersFileName");
		if (filename.isEmpty() || filename=="NoInitialTransform")
			break;
	}

	return mMf;
}


QString ElastixExecuter::getNonlinearResultVolume(bool* ok)
{
	if (ok)
		*ok = true;

	QString retval;
	int i=0;
	for (; ; ++i)
	{
		//TODO only mhd supported
		QString filename = QString(mLastOutdir + "/result.%1.mhd").arg(i);
		if (!QFileInfo(filename).exists())
			break;
		retval = filename;
	}

	if (retval.isEmpty())
		return retval;

	QString paramFilename = QString(mLastOutdir + "/TransformParameters.%1.txt").arg(i-1);
	ElastixParameterFile file(paramFilename);
	QString transform = file.readParameterString("Transform");

	if ((transform=="BSplineTransform") || (transform=="SplineKernelTransform"))
	{
		ssc::messageManager()->sendInfo(QString("Reading result file %1 created with transform %2").arg(retval).arg(transform));
		return retval;
	}
	else
	{
		if (ok)
			*ok = false;
		return "";
	}

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
	std::vector<double> tp = this->readParameterDoubleVector("TransformParameters");
	std::vector<double> cor = this->readParameterDoubleVector("CenterOfRotationPoint");

	ElastixEulerTransform E = ElastixEulerTransform::create(
		ssc::Vector3D(tp[0], tp[1], tp[2]),
		ssc::Vector3D(tp[3], tp[4], tp[5]),
		ssc::Vector3D(cor[0], cor[1], cor[2]));

	return E.toMatrix();
}

ssc::Transform3D ElastixParameterFile::readAffineTransform()
{
	QString transformType = this->readParameterString("Transform");
	if (transformType!="AffineTransform")
		ssc::messageManager()->sendError("Assert failure: attempting to read AffineTransform");

	int numberOfParameters = this->readParameterInt("NumberOfParameters");
	if (numberOfParameters!=12)
	{
		ssc::messageManager()->sendWarning(QString("Expected 12 Euler parameters, got %1").arg(numberOfParameters));
		return ssc::Transform3D::Identity();
	}
	std::vector<double> tp = this->readParameterDoubleVector("TransformParameters");
//	std::vector<double> cor = this->readParameterDoubleVector("CenterOfRotationPoint");

	ssc::Transform3D M = ssc::Transform3D::Identity();

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
		ssc::messageManager()->sendWarning(QString("Can't open ElastiX result file %1").arg(filename));
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

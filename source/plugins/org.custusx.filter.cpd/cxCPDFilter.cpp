/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxCPDFilter.h"

#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>

#include <vtkPolyData.h>
#include <vtkPoints.h>

#include "cxMesh.h"
#include "cxLog.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxSelectDataStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxDataLocations.h"
#include "cxRegistrationTransform.h"
#include "cxRegistrationApplicator.h"
#include "cxTypeConversions.h"

namespace cx
{

CPDFilter::CPDFilter(VisServicesPtr services) :
	FilterImpl(services),
	mDeltaRMd(Transform3D::Identity())
{
}

QString CPDFilter::getType() const
{
	return "cpd_filter";
}

QString CPDFilter::getName() const
{
	return "Coherent Point Drift";
}

QString CPDFilter::getHelp() const
{
	return "<html>"
		"<h3>Coherent Point Drift (CPD)</h3>"
		"<p>Registers two meshes using the Coherent Point Drift rigid algorithm "
		"via the pycpd Python library.</p>"
		"<p>The computed rigid transform is applied as a registration to the moving mesh, "
		"so all data in the same frame tree moves together.</p>"
		"<p>Requires a Python virtual environment with pycpd. "
		"The environment is created automatically on first use.</p>"
		"<p>The fixed mesh is the target (stays unchanged). "
		"The moving mesh is registered to match the fixed mesh.</p>"
		"</html>";
}

DoublePropertyPtr CPDFilter::getMaxIterationsOption(QDomElement root)
{
	return DoubleProperty::initialize("Max iterations", "",
		"Maximum number of EM iterations", 100, DoubleRange(1, 1000, 1), 0, root);
}

DoublePropertyPtr CPDFilter::getToleranceOption(QDomElement root)
{
	return DoubleProperty::initialize("Tolerance", "",
		"Convergence tolerance (smaller = more precise)", 1e-5, DoubleRange(1e-8, 1e-2, 1e-8), 8, root);
}

void CPDFilter::createOptions()
{
	mOptionsAdapters.push_back(getMaxIterationsOption(mOptions));
	mOptionsAdapters.push_back(getToleranceOption(mOptions));
}

void CPDFilter::createInputTypes()
{
	StringPropertySelectMeshPtr fixed = StringPropertySelectMesh::New(mServices->patient());
	fixed->setValueName("Fixed mesh");
	fixed->setHelp("Target mesh — stays unchanged");
	mInputTypes.push_back(fixed);

	StringPropertySelectMeshPtr moving = StringPropertySelectMesh::New(mServices->patient());
	moving->setValueName("Moving mesh");
	moving->setHelp("Source mesh — will be registered to the fixed mesh");
	mInputTypes.push_back(moving);
}

void CPDFilter::createOutputTypes()
{
}

bool CPDFilter::writeMeshPoints(vtkPolyDataPtr polyData, const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		CX_LOG_ERROR() << "CPDFilter: Cannot write point file: " << filePath;
		return false;
	}

	QTextStream stream(&file);
	vtkPoints* points = polyData->GetPoints();
	if (!points)
	{
		CX_LOG_ERROR() << "CPDFilter: Mesh has no points";
		return false;
	}

	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
	{
		double p[3];
		points->GetPoint(i, p);
		stream << p[0] << " " << p[1] << " " << p[2] << "\n";
	}
	return true;
}

bool CPDFilter::readTransform(const QString& filePath, Transform3D& deltaRMd)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		CX_LOG_ERROR() << "CPDFilter: Cannot read transform file: " << filePath;
		return false;
	}

	QTextStream stream(&file);
	Eigen::Matrix3d R;
	Eigen::Vector3d t;

	for (int row = 0; row < 3; ++row)
	{
		stream >> R(row, 0) >> R(row, 1) >> R(row, 2);
		if (stream.status() != QTextStream::Ok)
		{
			CX_LOG_ERROR() << "CPDFilter: Malformed transform file at R row " << row;
			return false;
		}
	}

	stream >> t(0) >> t(1) >> t(2);
	if (stream.status() != QTextStream::Ok)
	{
		CX_LOG_ERROR() << "CPDFilter: Malformed transform file for translation";
		return false;
	}

	deltaRMd.setIdentity();
	deltaRMd.linear() = R;
	deltaRMd.translation() = t;
	return true;
}

QString CPDFilter::findCPDScript() const
{
	QString scriptPath = DataLocations::getFilterScriptsPath() + "scripts/python_cpd/cpd_registration.py";
	if (QFileInfo::exists(scriptPath))
		return scriptPath;

	CX_LOG_ERROR() << "CPDFilter: Script not found at " << scriptPath;
	return QString();
}

QString CPDFilter::getVenvPythonPath() const
{
	QString venvBase = DataLocations::getVirtualEnvironmentsPath() + "/pycpd/venv";
#ifdef CX_WINDOWS
	return venvBase + "/Scripts/python.exe";
#else
	return venvBase + "/bin/python";
#endif
}

bool CPDFilter::ensureVenv()
{
	QString pythonPath = getVenvPythonPath();
	if (QFileInfo::exists(pythonPath))
		return true;

	QString venvBasePath = DataLocations::getVirtualEnvironmentsPath() + "/pycpd";
	if (!QDir().mkpath(venvBasePath))
	{
		CX_LOG_ERROR() << "CPDFilter: Cannot create directory: " << venvBasePath;
		return false;
	}

	QString createScript = DataLocations::getFilterScriptsPath() + "cxCreateVenv.sh";
	if (!QFileInfo::exists(createScript))
	{
		CX_LOG_ERROR() << "CPDFilter: Venv creation script not found: " << createScript;
		return false;
	}

	// QMessageBox must run on the GUI thread; this method is called from a worker thread
	QMetaObject::invokeMethod(qApp, [venvBasePath]()
	{
		QMessageBox messageBox;
		messageBox.setWindowModality(Qt::WindowModal);
		messageBox.setTextFormat(Qt::RichText);
		messageBox.setText("Virtual environment missing");
		messageBox.setInformativeText(
			"There is no virtual environment for pycpd at:<br><code>" + venvBasePath + "</code><br><br>"
			"CustusX will create one now by running <code>pip install pycpd</code>.");
		messageBox.exec();
	}, Qt::BlockingQueuedConnection);

	CX_LOG_INFO() << "CPDFilter: Creating pycpd venv at " << venvBasePath;

	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start("bash", QStringList() << createScript << venvBasePath << "pycpd");

	if (!process.waitForStarted(10000))
	{
		CX_LOG_ERROR() << "CPDFilter: Failed to start venv creation script";
		return false;
	}

	process.waitForFinished(5 * 60 * 1000);

	QByteArray output = process.readAllStandardOutput();
	if (!output.isEmpty())
		CX_LOG_INFO() << "CPD venv: " << QString(output).trimmed();

	if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
	{
		CX_LOG_ERROR() << "CPDFilter: Venv creation failed with exit code " << process.exitCode();
		return false;
	}

	if (!QFileInfo::exists(pythonPath))
	{
		CX_LOG_ERROR() << "CPDFilter: Python binary not found after venv creation: " << pythonPath;
		return false;
	}

	CX_LOG_INFO() << "CPDFilter: Virtual environment created successfully";
	return true;
}

bool CPDFilter::execute()
{
	MeshPtr fixedMesh = boost::dynamic_pointer_cast<Mesh>(mCopiedInput[0]);
	MeshPtr movingMesh = boost::dynamic_pointer_cast<Mesh>(mCopiedInput[1]);

	if (!fixedMesh || !movingMesh)
	{
		CX_LOG_ERROR() << "CPDFilter: Missing mesh input(s)";
		return false;
	}

	// Transform to reference (world) space so both meshes share a common coordinate frame
	vtkPolyDataPtr fixedPoly = fixedMesh->getTransformedPolyDataCopy(fixedMesh->get_rMd());
	vtkPolyDataPtr movingPoly = movingMesh->getTransformedPolyDataCopy(movingMesh->get_rMd());

	if (!fixedPoly || !movingPoly)
	{
		CX_LOG_ERROR() << "CPDFilter: Mesh(es) have no polydata";
		return false;
	}

	QString scriptPath = findCPDScript();
	if (scriptPath.isEmpty())
		return false;

	if (!ensureVenv())
		return false;

	QString patientFolder = mServices->patient()->getActivePatientFolder();
	QString fixedPtsFile = patientFolder + "/cpd_fixed_pts.txt";
	QString movingPtsFile = patientFolder + "/cpd_moving_pts.txt";
	QString resultFile = patientFolder + "/cpd_result_transform.txt";

	if (!writeMeshPoints(fixedPoly, fixedPtsFile))
		return false;
	if (!writeMeshPoints(movingPoly, movingPtsFile))
		return false;

	DoublePropertyPtr maxIterOption = getMaxIterationsOption(mCopiedOptions);
	DoublePropertyPtr tolOption = getToleranceOption(mCopiedOptions);

	int maxIter = static_cast<int>(maxIterOption->getValue());
	double tolerance = tolOption->getValue();

	QStringList args;
	args << scriptPath
		 << fixedPtsFile
		 << movingPtsFile
		 << resultFile
		 << QString::number(maxIter)
		 << QString::number(tolerance, 'g', 10);

	CX_LOG_INFO() << "CPDFilter: Running rigid CPD registration...";

	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start(getVenvPythonPath(), args);

	if (!process.waitForStarted(10000))
	{
		CX_LOG_ERROR() << "CPDFilter: Failed to start Python from venv: " << getVenvPythonPath();
		return false;
	}

	while (process.state() != QProcess::NotRunning)
	{
		process.waitForReadyRead(500);
		QByteArray output = process.readAllStandardOutput();
		if (!output.isEmpty())
			CX_LOG_INFO() << "CPD: " << QString(output).trimmed();
	}
	process.waitForFinished(60 * 60 * 1000);

	QByteArray remaining = process.readAllStandardOutput();
	if (!remaining.isEmpty())
		CX_LOG_INFO() << "CPD: " << QString(remaining).trimmed();

	if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
	{
		CX_LOG_ERROR() << "CPDFilter: Python script failed with exit code " << process.exitCode();
		return false;
	}

	if (!readTransform(resultFile, mDeltaRMd))
		return false;

	CX_LOG_INFO() << "CPDFilter: Registration complete";
	return true;
}

bool CPDFilter::postProcess()
{
	MeshPtr movingMesh = boost::dynamic_pointer_cast<Mesh>(mInputTypes[1]->getData());
	MeshPtr fixedMesh = boost::dynamic_pointer_cast<Mesh>(mInputTypes[0]->getData());
	if (!movingMesh || !fixedMesh)
		return false;

	RegistrationTransform regTrans(mDeltaRMd, QDateTime::currentDateTime(), "CPD registration");
	regTrans.mFixed = fixedMesh->getUid();
	regTrans.mMoving = movingMesh->getUid();

	RegistrationApplicator applicator(mServices->patient()->getDatas());
	applicator.updateRegistration(QDateTime(), regTrans);

	mDeltaRMd = Transform3D::Identity();
	return true;
}

} // namespace cx

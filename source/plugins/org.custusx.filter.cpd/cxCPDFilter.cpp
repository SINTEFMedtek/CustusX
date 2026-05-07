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

#include <vtkPolyData.h>
#include <vtkPoints.h>

#include "cxMesh.h"
#include "cxLog.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxSelectDataStringProperty.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxDataLocations.h"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include "cxViewService.h"

namespace cx
{

CPDFilter::CPDFilter(VisServicesPtr services) :
	FilterImpl(services)
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
		"<p>Registers two meshes using the Coherent Point Drift algorithm "
		"via the pycpd Python library.</p>"
		"<p>Requires Python 3 with pycpd installed: <code>pip install pycpd</code></p>"
		"<p>The fixed mesh is the target (stays unchanged). "
		"The moving mesh is warped to match the fixed mesh.</p>"
		"</html>";
}

StringPropertyPtr CPDFilter::getRegistrationTypeOption(QDomElement root)
{
	QStringList list;
	list << "rigid" << "affine" << "deformable";
	return StringProperty::initialize("Registration type", "",
		"rigid: rotation+translation, affine: adds scaling/shear, deformable: non-rigid (GMM)",
		list[0], list, root);
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
	mOptionsAdapters.push_back(getRegistrationTypeOption(mOptions));
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
	moving->setHelp("Source mesh — will be warped to match the fixed mesh");
	mInputTypes.push_back(moving);
}

void CPDFilter::createOutputTypes()
{
	StringPropertySelectMeshPtr output = StringPropertySelectMesh::New(mServices->patient());
	output->setValueName("Output mesh");
	output->setHelp("Registered moving mesh");
	mOutputTypes.push_back(output);
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

bool CPDFilter::readResultPoints(vtkPolyDataPtr polyData, const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		CX_LOG_ERROR() << "CPDFilter: Cannot read result file: " << filePath;
		return false;
	}

	QTextStream stream(&file);
	vtkPoints* points = polyData->GetPoints();
	vtkIdType expectedCount = points->GetNumberOfPoints();
	vtkIdType count = 0;

	while (!stream.atEnd() && count < expectedCount)
	{
		QString line = stream.readLine().trimmed();
		if (line.isEmpty())
			continue;
		QStringList parts = line.split(" ", QString::SkipEmptyParts);
		if (parts.size() < 3)
			continue;
		points->SetPoint(count++, parts[0].toDouble(), parts[1].toDouble(), parts[2].toDouble());
	}

	points->Modified();
	polyData->Modified();

	if (count != expectedCount)
	{
		CX_LOG_ERROR() << "CPDFilter: Expected " << expectedCount << " result points, got " << count;
		return false;
	}
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

	QString patientFolder = mServices->patient()->getActivePatientFolder();
	QString fixedPtsFile = patientFolder + "/cpd_fixed_pts.txt";
	QString movingPtsFile = patientFolder + "/cpd_moving_pts.txt";
	QString resultPtsFile = patientFolder + "/cpd_result_pts.txt";

	if (!writeMeshPoints(fixedPoly, fixedPtsFile))
		return false;
	if (!writeMeshPoints(movingPoly, movingPtsFile))
		return false;

	StringPropertyPtr regTypeOption = getRegistrationTypeOption(mCopiedOptions);
	DoublePropertyPtr maxIterOption = getMaxIterationsOption(mCopiedOptions);
	DoublePropertyPtr tolOption = getToleranceOption(mCopiedOptions);

	QString regType = regTypeOption->getValue();
	int maxIter = static_cast<int>(maxIterOption->getValue());
	double tolerance = tolOption->getValue();

	QStringList args;
	args << scriptPath
		 << fixedPtsFile
		 << movingPtsFile
		 << resultPtsFile
		 << regType
		 << QString::number(maxIter)
		 << QString::number(tolerance, 'g', 10);

	CX_LOG_INFO() << "CPDFilter: Running " << regType << " CPD registration...";

	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start("python3", args);

	if (!process.waitForStarted(10000))
	{
		CX_LOG_ERROR() << "CPDFilter: Failed to start Python. Is python3 available?";
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
		CX_LOG_ERROR() << "CPDFilter: " << QString(process.readAllStandardError());
		return false;
	}

	// Deep-copy moving mesh polydata and update point positions with CPD result
	vtkPolyDataPtr resultPoly = vtkPolyDataPtr::New();
	resultPoly->DeepCopy(movingPoly);

	if (!readResultPoints(resultPoly, resultPtsFile))
		return false;

	mResultPolyData = resultPoly;

	CX_LOG_INFO() << "CPDFilter: Registration complete";
	return true;
}

bool CPDFilter::postProcess()
{
	if (!mResultPolyData)
		return false;

	MeshPtr movingMesh = boost::dynamic_pointer_cast<Mesh>(mInputTypes[1]->getData());
	if (!movingMesh)
		return false;

	QString uid = movingMesh->getUid() + "_cpd";
	QString name = movingMesh->getName() + "_cpd";
	MeshPtr outputMesh = patientService()->createSpecificData<Mesh>(uid, name);
	outputMesh->setVtkPolyData(mResultPolyData);
	outputMesh->setColor(movingMesh->getColor());
	patientService()->insertData(outputMesh);

	// Result points are in reference space (rMd = identity)
	outputMesh->get_rMd_History()->setRegistration(Transform3D::Identity());

	mServices->view()->autoShowData(outputMesh);
	mOutputTypes[0]->setValue(outputMesh->getUid());

	mResultPolyData = nullptr;
	return true;
}

} // namespace cx

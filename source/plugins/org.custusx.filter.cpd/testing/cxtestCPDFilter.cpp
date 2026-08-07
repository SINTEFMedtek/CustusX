/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxCPDFilter.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxVisServices.h"

#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTextStream>

#include <vector>

#include <vtkPolyData.h>
#include <vtkPoints.h>

namespace cxtest {

// Exposes the two protected methods for direct testing.
class TestableCPDFilter : public cx::CPDFilter
{
public:
	TestableCPDFilter(cx::VisServicesPtr services) : CPDFilter(services) {}

	bool callWriteMeshPoints(vtkPolyDataPtr poly, const QString& path)
	{
		return writeMeshPoints(poly, path);
	}

	bool callReadTransform(const QString& path, cx::Transform3D& deltaRMd)
	{
		return readTransform(path, deltaRMd);
	}
};

// ---------------------------------------------------------------------------
// Helpers

cx::VisServicesPtr initServices()
{
	cx::LogicManager::initialize();
	return cx::VisServices::create(cx::logicManager()->getPluginContext());
}

vtkPolyDataPtr makePolyDataWithPoints(std::initializer_list<std::vector<double> > pts)
{
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (const std::vector<double>& p : pts)
		points->InsertNextPoint(p[0], p[1], p[2]);
	vtkPolyDataPtr pd = vtkPolyDataPtr::New();
	pd->SetPoints(points);
	return pd;
}

// Writes a transform file in the format produced by the Python script:
//   3 rows of R_col (= R.T from pycpd), then 1 row of t.
void writeTransformFile(const QString& path, const Eigen::Matrix3d& R_col, const Eigen::Vector3d& t)
{
	QFile file(path);
	REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Text));
	QTextStream stream(&file);
	for (int row = 0; row < 3; ++row)
		stream << R_col(row, 0) << " " << R_col(row, 1) << " " << R_col(row, 2) << "\n";
	stream << t(0) << " " << t(1) << " " << t(2) << "\n";
}

// ---------------------------------------------------------------------------
// Tests

TEST_CASE("CPDFilter: instantiate and check script exists", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	cx::CPDFilterPtr filter = cx::CPDFilterPtr(new cx::CPDFilter(services));

	REQUIRE(filter);
	CHECK(filter->getType() == "cpd_filter");
	CHECK(!filter->getName().isEmpty());

	QString scriptPath = cx::DataLocations::getFilterScriptsPath() + "scripts/python_cpd/cpd_registration.py";
	INFO("Expected script at: " << scriptPath.toStdString());
	CHECK(QFileInfo::exists(scriptPath));

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: option defaults", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	cx::CPDFilter filter(services);

	QDomDocument doc;
	QDomElement root = doc.createElement("options");

	CHECK(filter.getMaxIterationsOption(root)->getValue() == Approx(100.0));
	CHECK(filter.getToleranceOption(root)->getValue() == Approx(1e-3));
	CHECK(filter.getOutlierWeightOption(root)->getValue() == Approx(0.0));
	CHECK(filter.getScaleModeOption(root)->getValue() == "Rigid");
	CHECK(filter.getScaleThresholdOption(root)->getValue() == Approx(0.1));

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: scale mode option has correct values", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	cx::CPDFilter filter(services);

	QDomDocument doc;
	QDomElement root = doc.createElement("options");
	cx::StringPropertyPtr scaleProp = filter.getScaleModeOption(root);

	CHECK(scaleProp->getValue() == "Rigid");
	scaleProp->setValue("Rigid+scale");
	CHECK(scaleProp->getValue() == "Rigid+scale");
	scaleProp->setValue("Auto");
	CHECK(scaleProp->getValue() == "Auto");

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: input and output type counts", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	cx::CPDFilter filter(services);

	CHECK(filter.getInputTypes().size() == 2);
	CHECK(filter.getOutputTypes().size() == 0);

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: writeMeshPoints writes correct point coordinates", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	TestableCPDFilter filter(services);

	vtkPolyDataPtr pd = makePolyDataWithPoints({{1.0, 2.0, 3.0}, {4.5, -6.0, 0.0}});

	QTemporaryFile tmpFile;
	REQUIRE(tmpFile.open());
	QString path = tmpFile.fileName();
	tmpFile.close();

	REQUIRE(filter.callWriteMeshPoints(pd, path));

	QFile file(path);
	REQUIRE(file.open(QIODevice::ReadOnly | QIODevice::Text));
	QTextStream stream(&file);

	double x, y, z;

	// First point
	stream >> x >> y >> z;
	REQUIRE(stream.status() == QTextStream::Ok);
	CHECK(x == Approx(1.0));
	CHECK(y == Approx(2.0));
	CHECK(z == Approx(3.0));

	// Second point
	stream >> x >> y >> z;
	REQUIRE(stream.status() == QTextStream::Ok);
	CHECK(x == Approx(4.5));
	CHECK(y == Approx(-6.0));
	CHECK(z == Approx(0.0));

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: writeMeshPoints returns false for null polydata", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	TestableCPDFilter filter(services);

	QTemporaryFile tmpFile;
	REQUIRE(tmpFile.open());
	QString path = tmpFile.fileName();
	tmpFile.close();

	vtkPolyDataPtr pd = vtkPolyDataPtr::New();  // has no points
	CHECK_FALSE(filter.callWriteMeshPoints(pd, path));

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: readTransform parses identity transform", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	TestableCPDFilter filter(services);

	QTemporaryFile tmpFile;
	REQUIRE(tmpFile.open());
	QString path = tmpFile.fileName();
	tmpFile.close();

	writeTransformFile(path, Eigen::Matrix3d::Identity(), Eigen::Vector3d::Zero());

	cx::Transform3D deltaRMd;
	REQUIRE(filter.callReadTransform(path, deltaRMd));

	// Identity rotation and zero translation → identity 4x4
	CHECK(deltaRMd.matrix().isApprox(Eigen::Matrix4d::Identity(), 1e-9));

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: readTransform parses rotation and translation", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	TestableCPDFilter filter(services);

	QTemporaryFile tmpFile;
	REQUIRE(tmpFile.open());
	QString path = tmpFile.fileName();
	tmpFile.close();

	// 90-degree rotation around Z axis (column-vector convention): R_col
	Eigen::Matrix3d R_col;
	R_col << 0, -1, 0,
	         1,  0, 0,
	         0,  0, 1;
	Eigen::Vector3d t(10.0, -5.0, 3.0);

	writeTransformFile(path, R_col, t);

	cx::Transform3D deltaRMd;
	REQUIRE(filter.callReadTransform(path, deltaRMd));

	CHECK(deltaRMd.linear().isApprox(R_col, 1e-9));
	CHECK(deltaRMd.translation().isApprox(t, 1e-9));

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: readTransform returns false for missing file", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	TestableCPDFilter filter(services);

	cx::Transform3D deltaRMd;
	CHECK_FALSE(filter.callReadTransform("/nonexistent/path/transform.txt", deltaRMd));

	cx::LogicManager::shutdown();
}


TEST_CASE("CPDFilter: readTransform returns false for malformed file", "[unit][org.custusx.filter.cpd]")
{
	cx::VisServicesPtr services = initServices();
	TestableCPDFilter filter(services);

	QTemporaryFile tmpFile;
	REQUIRE(tmpFile.open());
	tmpFile.write("1.0 2.0\n");  // Only 2 values on row 0, expects 3
	QString path = tmpFile.fileName();
	tmpFile.close();

	cx::Transform3D deltaRMd;
	CHECK_FALSE(filter.callReadTransform(path, deltaRMd));

	cx::LogicManager::shutdown();
}

} // namespace cxtest

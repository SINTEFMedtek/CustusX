/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QTabWidget>
#include <vtkPolyData.h>
#include "cxtestFakeReconstructionExtension.h"
#include "cxtestReconstructionManagerFixture.h"
#include "cxtestReconstructRealData.h"
#include "cxLogicManager.h"
#include "cxMesh.h"
#include "cxPatientModelServiceProxy.h"
#include "cxProperty.h"
#include "cxRegisteredService.h"
#include "cxReconstructionWidget.h"
#include "cxReconstructionResultsWidget.h"
#include "cxColorSelectButton.h"
#include "cxDataSelectWidget.h"
#include "cxUsReconstructionRunner.h"
#include "cxUsReconstructionService.h"
#include "cxViewService.h"

namespace cxtest
{

namespace
{
QVariant getParam(cx::UsReconstructionServicePtr reconstruction, QString uid)
{
	return reconstruction->getParam(uid)->getValueAsVariant();
}

bool runAndWait(cx::UsReconstructionRunner& runner, cx::UsReconstructionRunner::Outputs outputs, std::vector<cx::UsReconstructionRunner::ExtensionOutputs> extensions)
{
	QObject::connect(&runner, &cx::UsReconstructionRunner::finished, qApp, &QApplication::quit);
	bool retval = runner.start(outputs, extensions);
	if (retval && runner.isRunning())
	{
		qApp->exec();
	}
	return retval;
}
} // namespace

TEST_CASE("UsReconstructionRunner: sets the angio parameters from the selected outputs", "[unit][usreconstruction]")
{
	ReconstructionManagerTestFixture fixture;
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->getParam("Preset")->setValueFromVariant("US B-Mode");

	cx::UsReconstructionRunner::setAngioParameters(reconstruction, true, true);
	CHECK(getParam(reconstruction, "Angio data").toBool());
	CHECK(getParam(reconstruction, "Dual Angio").toBool());
	CHECK(getParam(reconstruction, "Preset").toString() == "US Angio");

	cx::UsReconstructionRunner::setAngioParameters(reconstruction, false, true);
	CHECK(getParam(reconstruction, "Angio data").toBool());
	CHECK_FALSE(getParam(reconstruction, "Dual Angio").toBool());

	cx::UsReconstructionRunner::setAngioParameters(reconstruction, true, false);
	CHECK_FALSE(getParam(reconstruction, "Angio data").toBool());
	CHECK(getParam(reconstruction, "Preset").toString() == "US B-Mode");
}

TEST_CASE("UsReconstructionRunner: runs the extensions and lists the created data", "[unit][usreconstruction]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::UsReconstructionRunner runner(reconstruction, patientModel);
	FakeReconstructionExtension extension(patientModel);
	cx::UsReconstructionRunner::Outputs outputs;
	outputs.mUsVolume = false;
	cx::UsReconstructionRunner::ExtensionOutputs extensionOutputs;
	extensionOutputs.mExtension = &extension;
	extensionOutputs.mOutputs << "fakeB";
	std::vector<cx::UsReconstructionRunner::ExtensionOutputs> extensions(1, extensionOutputs);

	CHECK_FALSE(runner.start(outputs, extensions));

	reconstruction->selectData(realData.getSourceFilename());
	REQUIRE(runAndWait(runner, outputs, extensions));
	CHECK_FALSE(runner.isRunning());
	CHECK(extension.mStarted == QStringList() << "fakeB");
	REQUIRE(runner.getCreatedData().size() == 1);
	CHECK(runner.getCreatedData()[0]->getUid() == "fake_fakeB");
}

TEST_CASE("UsReconstructionRunner: reconstructs the US and angio volumes and the angio mesh", "[integration][usreconstruction][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->selectData(realData.getSourceFilename());
	reconstruction->getParam("Algorithm")->setValueFromVariant("pnn");
	fixture.setPNN_InterpolationSteps(1);
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::UsReconstructionRunner runner(reconstruction, patientModel);
	cx::UsReconstructionRunner::Outputs outputs;
	outputs.mAngioMesh = true;

	REQUIRE(runAndWait(runner, outputs, std::vector<cx::UsReconstructionRunner::ExtensionOutputs>()));
	std::vector<cx::DataPtr> created = runner.getCreatedData();
	REQUIRE(created.size() == 3);
	int images = 0;
	cx::MeshPtr mesh;
	for (unsigned i=0; i<created.size(); ++i)
	{
		images += std::dynamic_pointer_cast<cx::Image>(created[i]) ? 1 : 0;
		mesh = std::dynamic_pointer_cast<cx::Mesh>(created[i]) ? std::dynamic_pointer_cast<cx::Mesh>(created[i]) : mesh;
	}
	CHECK(images == 2);
	REQUIRE(mesh);
	CHECK(mesh->getVtkPolyData()->GetNumberOfPoints() > 0);
	CHECK(patientModel->getData<cx::Image>(mesh->getParentSpace())->getImageType() == cx::istANGIO);
}

TEST_CASE("ReconstructionWidget: adds the outputs and the settings tab of an extension", "[unit][usreconstruction][gui]")
{
	ReconstructionManagerTestFixture fixture;
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	ctkPluginContext* context = cx::logicManager()->getPluginContext();
	cx::ReconstructionWidget widget(NULL, fixture.getManager(), patientModel, cx::ViewService::getNullObject(), context);
	QTabWidget* tabs = widget.findChild<QTabWidget*>();
	REQUIRE(tabs);
	int tabCount = tabs->count();
	CHECK(tabCount >= 2);
	CHECK(tabs->tabText(0) == "Reconstruct");
	CHECK(tabs->tabText(1) == "US settings");

	cx::RegisteredServicePtr registered(new cx::RegisteredService(context, new FakeReconstructionExtension(patientModel), UsReconstructionExtensionService_iid));
	CHECK(tabs->count() == tabCount + 1);
	CHECK(tabs->tabText(tabCount) == "Fake");
	CHECK(widget.findChild<QCheckBox*>("fakeA"));
	CHECK(widget.findChild<QCheckBox*>("usVolume"));

	registered.reset();
	QCoreApplication::sendPostedEvents(NULL, QEvent::DeferredDelete);
	CHECK(tabs->count() == tabCount);
	CHECK_FALSE(widget.findChild<QCheckBox*>("fakeA"));
}

TEST_CASE("ReconstructionWidget: the angio checkboxes set the Angio data parameter", "[unit][usreconstruction][gui]")
{
	ReconstructionManagerTestFixture fixture;
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::ReconstructionWidget widget(NULL, fixture.getManager(), patientModel, cx::ViewService::getNullObject(), NULL);
	QCheckBox* angioVolume = widget.findChild<QCheckBox*>("angioVolume");
	REQUIRE(angioVolume);
	cx::PropertyPtr angioData = fixture.getManager()->getParam("Angio data");

	widget.findChild<QCheckBox*>("angioMesh")->setChecked(false);
	angioVolume->setChecked(true);
	CHECK(angioData->getValueAsVariant().toBool());
	angioVolume->setChecked(false);
	CHECK_FALSE(angioData->getValueAsVariant().toBool());
}

TEST_CASE("ReconstructionResultsWidget: lists the data, and drops removed data", "[unit][usreconstruction][gui]")
{
	ReconstructionManagerTestFixture fixture;
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::ImagePtr image(new cx::Image("results_image", cx::generateVtkImageData(Eigen::Array3i(2, 2, 2), cx::Vector3D(1, 1, 1), 0), "Results image"));
	cx::MeshPtr mesh(new cx::Mesh("results_mesh", "Results mesh"));
	patientModel->insertData(image);
	patientModel->insertData(mesh);

	cx::ReconstructionResultsWidget widget(patientModel, cx::ViewService::getNullObject());
	QColor imageColor(0, 0, 255);
	widget.setColorFunctions([image, &imageColor](cx::DataPtr data) { return data == image ? imageColor : QColor(); },
							 [&imageColor](cx::DataPtr, QColor color) { imageColor = color; });
	CHECK(widget.isHidden());
	widget.setData(std::vector<cx::DataPtr>{image, mesh});
	CHECK(widget.getUids() == QStringList() << "results_image" << "results_mesh");
	CHECK_FALSE(widget.isHidden());
	CHECK(widget.findChildren<cx::ColorSelectButton*>().size() == 2);
	CHECK(widget.findChildren<cx::EraseDataToolButton*>().size() == 2);

	patientModel->removeData("results_image");
	CHECK(widget.getUids() == QStringList() << "results_mesh");
}

} // namespace cxtest

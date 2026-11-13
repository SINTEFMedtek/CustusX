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
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <vtkPolyData.h>
#include "cxtestFakeReconstructionExtension.h"
#include "cxtestReconstructionManagerFixture.h"
#include "cxtestReconstructRealData.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxSessionStorageService.h"
#include "cxMesh.h"
#include "cxPatientModelServiceProxy.h"
#include "cxProperty.h"
#include "cxRegisteredService.h"
#include "cxSettings.h"
#include "cxReconstructionWidget.h"
#include "cxReconstructionResultsWidget.h"
#include "cxColorSelectButton.h"
#include "cxDataSelectWidget.h"
#include "cxUsReconstructionRunner.h"
#include "cxUsReconstructionService.h"
#include "cxViewService.h"
#include "cxTimedAlgorithm.h"

namespace cxtest
{

namespace
{
QVariant getParam(cx::UsReconstructionServicePtr reconstruction, QString uid)
{
	return reconstruction->getParam(uid)->getValueAsVariant();
}

/** Fails if the run does not finish within the time limit (ms). */
bool waitForRun(cx::UsReconstructionRunner& runner, int timeLimit = 600000)
{
	QObject::connect(&runner, &cx::UsReconstructionRunner::finished, qApp, &QApplication::quit);
	QTimer timer;
	timer.setSingleShot(true);
	QObject::connect(&timer, &QTimer::timeout, qApp, &QApplication::quit);
	timer.start(timeLimit);
	if (runner.isRunning())
	{
		qApp->exec();
	}
	bool retval = !runner.isRunning();
	if (!retval)
	{
		runner.cancel();
		timer.start(timeLimit);
		qApp->exec();
	}
	return retval;
}

bool runAndWait(cx::UsReconstructionRunner& runner, cx::UsReconstructionRunner::Outputs outputs, std::vector<cx::UsReconstructionRunner::ExtensionOutputs> extensions)
{
	bool retval = runner.start(outputs, extensions);
	return retval && waitForRun(runner);
}
} // namespace

TEST_CASE("ReconstructionWidget: sets the angio parameters from the selected outputs", "[unit][usreconstruction]")
{
	ReconstructionManagerTestFixture fixture;
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->getParam("Preset")->setValueFromVariant("US B-Mode");

	cx::ReconstructionWidget::applyAngioParameters(reconstruction, true, true);
	CHECK(getParam(reconstruction, "Angio data").toBool());
	CHECK(getParam(reconstruction, "Dual Angio").toBool());
	CHECK(getParam(reconstruction, "Preset").toString() == "US Angio");

	cx::ReconstructionWidget::applyAngioParameters(reconstruction, false, true);
	CHECK(getParam(reconstruction, "Angio data").toBool());
	CHECK_FALSE(getParam(reconstruction, "Dual Angio").toBool());

	cx::ReconstructionWidget::applyAngioParameters(reconstruction, true, false);
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
	CHECK(extension.mRecording == realData.getSourceFilename());
	REQUIRE(runner.getCreatedData().size() == 1);
	CHECK(runner.getCreatedData()[0]->getUid() == "fake_fakeB");
}

TEST_CASE("UsReconstructionRunner: loading another patient cancels the run, and its data is not listed as created", "[unit][usreconstruction]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->selectData(realData.getSourceFilename());
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::UsReconstructionRunner runner(reconstruction, patientModel);
	FakeReconstructionExtension extension(patientModel);
	cx::UsReconstructionRunner::Outputs outputs;
	outputs.mUsVolume = false;
	cx::UsReconstructionRunner::ExtensionOutputs first;
	first.mExtension = &extension;
	first.mOutputs << "fakeA";
	cx::UsReconstructionRunner::ExtensionOutputs second = first;
	second.mOutputs = QStringList() << "fakeB";
	std::vector<cx::UsReconstructionRunner::ExtensionOutputs> extensions = {first, second};
	QString status;
	QObject::connect(&runner, &cx::UsReconstructionRunner::statusChanged, [&status](QString text) { status = text; });

	REQUIRE(runner.start(outputs, extensions));
	patientModel->insertData(cx::ImagePtr(new cx::Image("otherPatientImage", cx::generateVtkImageData(Eigen::Array3i(2, 2, 2), cx::Vector3D(1, 1, 1), 0), "other")));
	emit patientModel->patientChanged();
	REQUIRE(waitForRun(runner));

	CHECK(extension.mStarted == QStringList() << "fakeA");
	CHECK(runner.getCreatedData().empty());
	CHECK(status == "Cancelled, as another patient was loaded");
}

TEST_CASE("UsReconstructionRunner: a US volume is not added to a patient loaded while it is reconstructed", "[integration][usreconstruction][not_win32]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->selectData(realData.getSourceFilename());
	reconstruction->getParam("Algorithm")->setValueFromVariant("pnn");
	fixture.setPNN_InterpolationSteps(1);
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::UsReconstructionRunner runner(reconstruction, patientModel);
	QString status;
	QObject::connect(&runner, &cx::UsReconstructionRunner::statusChanged, [&status](QString text) { status = text; });

	REQUIRE(runner.start(cx::UsReconstructionRunner::Outputs(), std::vector<cx::UsReconstructionRunner::ExtensionOutputs>()));
	REQUIRE(runner.isReconstructingUs());
	cx::logicManager()->getSessionStorageService()->load(cx::DataLocations::getTestDataPath() + "/temp/test_other_patient.cx3");
	REQUIRE(waitForRun(runner));

	CHECK(patientModel->getDataOfType<cx::Image>().empty());
	CHECK(runner.getCreatedData().empty());
	CHECK(status == "Cancelled, as another patient was loaded");
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
	reconstruction->getParam("Preset")->setValueFromVariant("US B-Mode");
	reconstruction->getParam("Angio data")->setValueFromVariant(false);
	cx::UsReconstructionRunner runner(reconstruction, patientModel);
	cx::UsReconstructionRunner::Outputs outputs;
	outputs.mAngioMesh = true;
	FakeReconstructionExtension extension(patientModel);
	cx::UsReconstructionRunner::ExtensionOutputs extensionOutputs;
	extensionOutputs.mExtension = &extension;
	extensionOutputs.mOutputs << "fakeA";

	REQUIRE(runner.start(outputs, std::vector<cx::UsReconstructionRunner::ExtensionOutputs>(1, extensionOutputs)));
	CHECK(getParam(reconstruction, "Preset").toString() == "US B-Mode");
	CHECK_FALSE(getParam(reconstruction, "Angio data").toBool());
	reconstruction->selectData(cx::USReconstructInputData());
	REQUIRE(waitForRun(runner));
	CHECK(extension.mRecording == realData.getSourceFilename());
	std::vector<cx::DataPtr> created = runner.getCreatedData();
	REQUIRE(created.size() == 4);
	int images = 0;
	cx::MeshPtr mesh;
	for (unsigned i=0; i<created.size(); ++i)
	{
		images += std::dynamic_pointer_cast<cx::Image>(created[i]) ? 1 : 0;
		mesh = std::dynamic_pointer_cast<cx::Mesh>(created[i]) ? std::dynamic_pointer_cast<cx::Mesh>(created[i]) : mesh;
	}
	CHECK(images == 3);
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

TEST_CASE("ReconstructionWidget: the angio checkboxes follow the Angio data parameter, e.g. set by the preset", "[unit][usreconstruction][gui]")
{
	ReconstructionManagerTestFixture fixture;
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->getParam("Preset")->setValueFromVariant("US B-Mode");
	reconstruction->getParam("Angio data")->setValueFromVariant(false);
	cx::ReconstructionWidget widget(NULL, reconstruction, patientModel, cx::ViewService::getNullObject(), NULL);
	QCheckBox* angioVolume = widget.findChild<QCheckBox*>("angioVolume");
	REQUIRE(angioVolume);
	CHECK_FALSE(angioVolume->isChecked());

	reconstruction->getParam("Preset")->setValueFromVariant("US Angio");
	CHECK(getParam(reconstruction, "Angio data").toBool());
	CHECK(angioVolume->isChecked());
	reconstruction->getParam("Angio data")->setValueFromVariant(false);
	CHECK_FALSE(angioVolume->isChecked());
}

TEST_CASE("ReconstructionWidget: cannot start while a US reconstruction runs when it opens", "[integration][usreconstruction][gui]")
{
	ReconstructionManagerTestFixture fixture;
	ReconstructRealTestData realData;
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->selectData(realData.getSourceFilename());
	QObject::connect(reconstruction.get(), &cx::UsReconstructionService::reconstructFinished, qApp, &QApplication::quit);
	QTimer timer;
	timer.setSingleShot(true);
	QObject::connect(&timer, &QTimer::timeout, qApp, &QApplication::quit);
	timer.start(600000);
	reconstruction->startReconstruction();
	std::set<cx::TimedAlgorithmPtr> threads = reconstruction->getThreadedReconstruction();
	REQUIRE(threads.size() == 1);

	cx::ReconstructionWidget widget(NULL, reconstruction, patientModel, cx::ViewService::getNullObject(), NULL);
	QPushButton* button = widget.findChild<QPushButton*>("reconstruct");
	REQUIRE(button);
	if (!(*threads.begin())->isFinished())
	{
		CHECK_FALSE(button->isEnabled());
	}
	qApp->exec();
}

TEST_CASE("ReconstructionWidget: opening the widget does not change the angio parameters", "[unit][usreconstruction][gui]")
{
	ReconstructionManagerTestFixture fixture;
	cx::PatientModelServicePtr patientModel = cx::PatientModelServiceProxy::create(cx::logicManager()->getPluginContext());
	cx::UsReconstructionServicePtr reconstruction = fixture.getManager();
	reconstruction->getParam("Angio data")->setValueFromVariant(false);
	reconstruction->getParam("Dual Angio")->setValueFromVariant(false);
	QVariant savedAngioVolume = cx::settings()->value("reconstruction/create/angioVolume");
	cx::settings()->setValue("reconstruction/create/angioVolume", true);

	cx::ReconstructionWidget widget(NULL, reconstruction, patientModel, cx::ViewService::getNullObject(), NULL);
	CHECK_FALSE(widget.findChild<QCheckBox*>("angioVolume")->isChecked());
	CHECK_FALSE(reconstruction->getParam("Angio data")->getValueAsVariant().toBool());
	CHECK_FALSE(reconstruction->getParam("Dual Angio")->getValueAsVariant().toBool());
	cx::settings()->setValue("reconstruction/create/angioVolume", savedAngioVolume);
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

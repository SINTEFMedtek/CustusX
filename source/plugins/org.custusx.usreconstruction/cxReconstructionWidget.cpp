/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxReconstructionWidget.h"

#include <QSignalBlocker>
#include "cxReconstructOutputValueParamsInterfaces.h"
#include "cxLogger.h"
#include "cxHelperWidgets.h"
#include "cxTypeConversions.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxSettings.h"
#include "cxTimedAlgorithm.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxUsReconstructionService.h"
#include "cxPatientModelService.h"
#include "cxServiceTrackerListener.h"
#include "cxUsReconstructionExtensionService.h"
#include "cxUsReconstructionRunner.h"
#include "cxReconstructParams.h"
#include "cxReconstructionResultsWidget.h"

namespace cx
{

namespace
{
void setLayoutItemVisible(QLayoutItem* item, bool visible)
{
	QWidget* widget = item ? item->widget() : nullptr;
	QLayout* layout = item ? item->layout() : nullptr;
	if (widget)
	{
		widget->setVisible(visible);
	}
	else if (layout)
	{
		for (int i = 0; i < layout->count(); ++i)
		{
			setLayoutItemVisible(layout->itemAt(i), visible);
		}
	}
}

/** Show or hide all widgets (label and input) in a row of a grid layout */
void setGridRowVisible(QGridLayout* layout, int row, bool visible)
{
	for (int column = 0; column < layout->columnCount(); ++column)
	{
		setLayoutItemVisible(layout->itemAtPosition(row, column), visible);
	}
}
}

ReconstructionWidget::ReconstructionWidget(QWidget* parent, UsReconstructionServicePtr reconstructer, PatientModelServicePtr patientModelService, ViewServicePtr viewService, ctkPluginContext* pluginContext) :
	BaseWidget(parent, "us_reconstruction", "US Reconstruction"),
	mReconstructer(reconstructer),
	mPatientModelService(patientModelService),
	mUsReconstructing(false),
	mOptionsLayout(NULL)
{
	this->setToolTip("Reconstruct 3D US data from an acquired 2D sequence");
	mRunner = new UsReconstructionRunner(mReconstructer, mPatientModelService);
	mRunner->setParent(this);
	connect(mReconstructer.get(), &UsReconstructionService::reconstructAboutToStart, this, &ReconstructionWidget::reconstructAboutToStartSlot);
	connect(mReconstructer.get(), &UsReconstructionService::reconstructStarted, this, &ReconstructionWidget::reconstructStartedSlot);
	connect(mReconstructer.get(), &UsReconstructionService::reconstructFinished, this, &ReconstructionWidget::reconstructFinishedSlot);

	connect(mReconstructer.get(), &UsReconstructionService::paramsChanged, this, &ReconstructionWidget::paramsChangedSlot);
	connect(mReconstructer.get(), &UsReconstructionService::inputDataSelected, this, &ReconstructionWidget::inputDataSelected);
	connect(mReconstructer.get(), &UsReconstructionService::algorithmChanged, this, &ReconstructionWidget::repopulateAlgorithmGroup);
	connect(mRunner, &UsReconstructionRunner::progress, this, &ReconstructionWidget::progressChanged);
	connect(mRunner, &UsReconstructionRunner::finished, this, &ReconstructionWidget::runFinished);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->setMargin(0);
	mTabs = new QTabWidget(this);
	mTabs->addTab(this->createReconstructTab(viewService), "Reconstruct");
	mTabs->addTab(this->createSettingsTab(), "US settings");
	topLayout->addWidget(mTabs);

	connect(mRunner, &UsReconstructionRunner::statusChanged, mStatusLabel, &QLabel::setText);

	if (pluginContext)
	{
		mExtensionListener.reset(new ServiceTrackerListener<UsReconstructionExtensionService>(
				pluginContext,
				[this](UsReconstructionExtensionService* service) { this->onExtensionAdded(service); },
				std::function<void(UsReconstructionExtensionService*)>(),
				[this](UsReconstructionExtensionService* service) { this->onExtensionRemoved(service); }));
		mExtensionListener->open();
	}

	this->paramsChangedSlot();
	mUsReconstructing = this->isUsReconstructing();
	if (mUsReconstructing)
	{
		mTimedAlgorithmProgressBar->attach(mReconstructer->getThreadedReconstruction());
	}
	this->updateButtons();
}

ReconstructionWidget::~ReconstructionWidget()
{
	mExtensionListener.reset();
}

QWidget* ReconstructionWidget::createReconstructTab(ViewServicePtr viewService)
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(retval);

	QLabel* recordingLabel = new QLabel("2D US sequence", retval);
	recordingLabel->setToolTip("The recorded 2D ultrasound sequence to reconstruct from");
	layout->addWidget(recordingLabel);
	mFileSelectWidget = this->createFileSelectWidget(retval);
	layout->addWidget(mFileSelectWidget);
	this->createOutputCheckBoxes(retval);
	layout->addWidget(mOutputsLayout->parentWidget());

	mReconstructButton = new QPushButton("Reconstruct", retval);
	mReconstructButton->setObjectName("reconstruct");
	connect(mReconstructButton, &QPushButton::clicked, this, &ReconstructionWidget::reconstruct);
	layout->addWidget(mReconstructButton);

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar(retval);
	layout->addWidget(mTimedAlgorithmProgressBar);
	mProgressBar = new QProgressBar(retval);
	layout->addWidget(mProgressBar);
	mStatusLabel = new QLabel(retval);
	mStatusLabel->setWordWrap(true);
	layout->addWidget(mStatusLabel);

	mResultsWidget = new ReconstructionResultsWidget(mPatientModelService, viewService, retval);
	mResultsWidget->setColorFunctions([this](DataPtr data) { return this->getDataColor(data); },
									  [this](DataPtr data, QColor color) { this->setDataColor(data, color); });
	layout->addWidget(mResultsWidget);
	layout->addStretch();
	return retval;
}

FileSelectWidget* ReconstructionWidget::createFileSelectWidget(QWidget* parent)
{
	FileSelectWidget* retval = new FileSelectWidget(parent);
	connect(retval, &FileSelectWidget::fileSelected, this, &ReconstructionWidget::selectData);
	retval->setNameFilter(QStringList() << "*.fts");
	// The Select data dialog starts in the US_Acq folder of the active patient
	PatientModelServicePtr patientModelService = mPatientModelService;
	retval->setDialogFolderProvider([patientModelService]() -> QString
	{
		QString folder;
		if (patientModelService && patientModelService->isPatientValid())
		{
			folder = patientModelService->getActivePatientFolder() + "/US_Acq";
		}
		return folder;
	});
	connect(mReconstructer.get(), &UsReconstructionService::newInputDataAvailable, retval, &FileSelectWidget::refresh);
	connect(mReconstructer.get(), &UsReconstructionService::newInputDataPath, this, &ReconstructionWidget::updateFileSelectorPath);
	return retval;
}

/** The angio checkboxes start from, and set, the Angio data parameter, which also decides if 8 bit acquisitions record color. */
void ReconstructionWidget::createOutputCheckBoxes(QWidget* parent)
{
	QGroupBox* outputsGroup = new QGroupBox("Create", parent);
	mOutputsLayout = new QVBoxLayout(outputsGroup);
	bool angio = mReconstructer->getParam("Angio data")->getValueAsVariant().toBool();
	mUsVolumeCheckBox = this->createOutputCheckBox("usVolume", "3D US volume", "The B-mode ultrasound volume", true);
	mAngioVolumeCheckBox = this->createOutputCheckBox("angioVolume", "Angio volume", "A volume of the color (Doppler) data of the recording", angio);
	mAngioMeshCheckBox = this->createOutputCheckBox("angioMesh", "Angio mesh", "A mesh of the angio volume, at the angio mesh threshold. Also creates the angio volume", false);
	this->showAngioData(angio);
	QList<QCheckBox*> checkBoxes = this->getUsCheckBoxes();
	for (int i = 0; i < checkBoxes.size(); ++i)
	{
		connect(checkBoxes[i], &QCheckBox::toggled, this, &ReconstructionWidget::setAngioParameters);
	}
	connect(mReconstructer->getParam("Angio data").get(), &Property::changed, this, &ReconstructionWidget::angioDataChanged);
}

QList<QCheckBox*> ReconstructionWidget::getUsCheckBoxes() const
{
	return QList<QCheckBox*>() << mUsVolumeCheckBox << mAngioVolumeCheckBox << mAngioMeshCheckBox;
}

/** E.g. the US Angio preset turns on Angio data. */
void ReconstructionWidget::angioDataChanged()
{
	this->showAngioData(mReconstructer->getParam("Angio data")->getValueAsVariant().toBool());
	this->updateButtons();
}

/** The saved angio checkboxes are only kept if they agree with Angio data. */
void ReconstructionWidget::showAngioData(bool angio)
{
	QSignalBlocker volumeBlocker(mAngioVolumeCheckBox);
	QSignalBlocker meshBlocker(mAngioMeshCheckBox);
	bool anyChecked = mAngioVolumeCheckBox->isChecked() || mAngioMeshCheckBox->isChecked();
	if (!angio)
	{
		mAngioVolumeCheckBox->setChecked(false);
		mAngioMeshCheckBox->setChecked(false);
	}
	else if (!anyChecked)
	{
		mAngioVolumeCheckBox->setChecked(true);
	}
}

void ReconstructionWidget::setAngioParameters()
{
	bool angio = mAngioVolumeCheckBox->isChecked() || mAngioMeshCheckBox->isChecked();
	applyAngioParameters(mReconstructer, mUsVolumeCheckBox->isChecked(), angio);
}

void ReconstructionWidget::applyAngioParameters(UsReconstructionServicePtr reconstruction, bool usVolume, bool angio)
{
	PropertyPtr preset = reconstruction->getParam("Preset");
	preset->setValueFromVariant(ReconstructParams::getPreset(preset->getValueAsVariant().toString(), angio));
	reconstruction->getParam("Angio data")->setValueFromVariant(angio);
	reconstruction->getParam("Dual Angio")->setValueFromVariant(usVolume);
}

QCheckBox* ReconstructionWidget::createOutputCheckBox(QString uid, QString name, QString help, bool defaultValue)
{
	QCheckBox* retval = new QCheckBox(name, this);
	retval->setObjectName(uid);
	retval->setToolTip(help);
	retval->setChecked(settings()->value("reconstruction/create/" + uid, defaultValue).toBool());
	connect(retval, &QCheckBox::toggled, this, &ReconstructionWidget::updateButtons);
	mOutputsLayout->addWidget(retval);
	return retval;
}

QWidget* ReconstructionWidget::createSettingsTab()
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* topLayout = new QVBoxLayout(retval);

	QHBoxLayout* extentLayout = new QHBoxLayout;
	mExtentLineEdit = this->createIndicator(retval);
	extentLayout->addWidget(new QLabel("Extent", retval));
	extentLayout->addWidget(mExtentLineEdit);

	mInputSpacingLineEdit = this->createIndicator(retval);
	QLabel* inputSpacingLabel = new QLabel("Spacing In", retval);

	QGridLayout* sizesLayout = new QGridLayout;
	QWidget* maxVolSizeWidget = sscCreateDataWidget(retval, mReconstructer->getParam("Volume Size"));
	sizesLayout->addWidget(inputSpacingLabel, 0, 0);
	sizesLayout->addWidget(mInputSpacingLineEdit, 0, 1);
	sizesLayout->addLayout(extentLayout, 0, 2);
	mOutputSpacingLineEdit = this->createIndicator(retval);
	sizesLayout->addWidget(new QLabel("Spacing Out", retval), 1, 0);
	sizesLayout->addWidget(mOutputSpacingLineEdit, 1, 1);
	sizesLayout->addWidget(maxVolSizeWidget, 1, 2);
	topLayout->addLayout(sizesLayout);

	topLayout->addWidget(sscCreateDataWidget(retval, mReconstructer->getParam("Preset")));
	topLayout->addWidget(sscCreateDataWidget(retval, mReconstructer->getParam("Reduce mask (% in 1D)")));

	mDimXLineEdit = this->createIndicator(retval);
	mDimYLineEdit = this->createIndicator(retval);
	mDimZLineEdit = this->createIndicator(retval);
	QHBoxLayout* outputVolDimLayout = new QHBoxLayout;
	outputVolDimLayout->addWidget(new QLabel("X", retval));
	outputVolDimLayout->addWidget(mDimXLineEdit);
	outputVolDimLayout->addWidget(new QLabel("Y", retval));
	outputVolDimLayout->addWidget(mDimYLineEdit);
	outputVolDimLayout->addWidget(new QLabel("Z", retval));
	outputVolDimLayout->addWidget(mDimZLineEdit);
	topLayout->addLayout(outputVolDimLayout);

	topLayout->addWidget(this->createOptionsWidget(retval));
	topLayout->addStretch();
	return retval;
}

QLineEdit* ReconstructionWidget::createIndicator(QWidget* parent)
{
	QLineEdit* retval = new QLineEdit(parent);
	retval->setReadOnly(true);
	return retval;
}

void ReconstructionWidget::updateFileSelectorPath(QString path)
{
	mFileSelectWidget->setPath(path);
	mFileSelectWidget->refresh();
}

QWidget* ReconstructionWidget::createOptionsWidget(QWidget* parent)
{
	QWidget* retval = new QWidget(parent);
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);
	mOptionsLayout = layout;

	int line = 0;

	layout->addWidget(this->createHorizontalLine(), line, 0, 1, 2);
	++line;

	mAlgorithmGroup = new QFrame(retval);
	mAlgorithmGroup->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	mAlgorithmGroup->setFocusPolicy(Qt::StrongFocus); // needed for help system: focus is used to display help text
	mAlgorithmGroup->setSizePolicy(mAlgorithmGroup->sizePolicy().horizontalPolicy(),QSizePolicy::Fixed);

	QVBoxLayout* algoOuterLayout = new QVBoxLayout(mAlgorithmGroup);
	QWidget* algorithmWidget = sscCreateDataWidget(retval, mReconstructer->getParam("Algorithm"));
	algoOuterLayout->addWidget(algorithmWidget);
	mAlgoLayout = new QStackedLayout;
	this->repopulateAlgorithmGroup();
	mAlgoLayout->setMargin(0);
	algoOuterLayout->addLayout(mAlgoLayout);
	layout->addWidget(mAlgorithmGroup, line, 0, 1, 2);
	++line;

	sscCreateDataWidget(retval, mReconstructer->getParam("Orientation"), layout, line++);
	sscCreateDataWidget(retval, mReconstructer->getParam("Flip images"), layout, line++);
	sscCreateDataWidget(retval, mReconstructer->getParam("Angio mesh threshold"), layout, line++);
	layout->addWidget(this->createHorizontalLine(), line++, 0, 1, 2);

	sscCreateDataWidget(retval, mReconstructer->getParam("Align timestamps"), layout, line++);
	sscCreateDataWidget(retval, mReconstructer->getParam("Extra Temporal Calib"), layout, line++);
	sscCreateDataWidget(retval, mReconstructer->getParam("Position Filter"), layout, line++);
	mMovingAverageRows.clear();
	mHoltRows.clear();
	mMovingAverageRows.push_back(line);
	sscCreateDataWidget(retval, mReconstructer->getParam("Position Filter Strength"), layout, line++);
	QStringList holtParameters = QStringList() << "Holt alpha" << "Holt beta" << "Holt both directions" << "Holt add trend";
	for (int i = 0; i < holtParameters.size(); ++i)
	{
		mHoltRows.push_back(line);
		sscCreateDataWidget(retval, mReconstructer->getParam(holtParameters[i]), layout, line++);
	}
	sscCreateDataWidget(retval, mReconstructer->getParam("Save processed positions"), layout, line++);

	// The parameters that are shown or enabled depend on other parameters
	QStringList controllingParameters = QStringList() << "Position Filter" << "Align timestamps";
	for (int i = 0; i < controllingParameters.size(); ++i)
	{
		connect(mReconstructer->getParam(controllingParameters[i]).get(), &Property::changed, this, &ReconstructionWidget::updateParameterStates);
	}
	this->updateParameterStates();

	return retval;
}

void ReconstructionWidget::updateParameterStates()
{
	if (mOptionsLayout)
	{
		// Show only the parameters of the selected position filter
		QString filter = mReconstructer->getParam("Position Filter")->getValueAsVariant().toString();
		for (unsigned i = 0; i < mMovingAverageRows.size(); ++i)
		{
			setGridRowVisible(mOptionsLayout, mMovingAverageRows[i], filter == "Moving average");
		}
		for (unsigned i = 0; i < mHoltRows.size(); ++i)
		{
			setGridRowVisible(mOptionsLayout, mHoltRows[i], filter == "Holt");
		}
	}

	// An estimated time lag that is applied replaces the user's time calibration, and aligned time stamps make an extra calibration meaningless
	bool applyLag = mApplyLagProperty && mApplyLagProperty->getValueAsVariant().toBool();
	PropertyPtr align = mReconstructer->getParam("Align timestamps");
	PropertyPtr extra = mReconstructer->getParam("Extra Temporal Calib");
	align->setEnabled(!applyLag);
	extra->setEnabled(!applyLag && !align->getValueAsVariant().toBool());
}

void ReconstructionWidget::repopulateAlgorithmGroup()
{
	QString algoName = mReconstructer->getParam("Algorithm")->getValueAsVariant().toString();

	if (mAlgoLayout->currentWidget() && (algoName == mAlgoLayout->currentWidget()->objectName()))
		return;

	for (int i=0; i<mAlgoLayout->count(); ++i)
	{
		if (algoName==mAlgoLayout->widget(i)->objectName())
		{
			mAlgoLayout->setCurrentIndex(i);
			return;
		}
	}

	// Remove previous widget: this ensures that the widget wraps tightly around the current
	// widget instead of wrapping all algorithms.
	QLayoutItem *child;
	while ((child = mAlgoLayout->takeAt(0)) != 0)
	{
		// delete both the layoutitem AND the widget. Not auto done because layoutitem is no QObject.
		QWidget* widget = child->widget();
		delete child;
		delete widget;
	}

	this->createNewStackedWidget(algoName);
}

void ReconstructionWidget::createNewStackedWidget(QString algoName)
{
	QWidget* oneAlgoWidget = new QWidget(this);
	oneAlgoWidget->setObjectName(algoName);
	mAlgorithmGroup->setObjectName(algoName);
	mAlgoLayout->addWidget(oneAlgoWidget);
	QGridLayout* oneAlgoLayout = new QGridLayout(oneAlgoWidget);
	oneAlgoLayout->setMargin(0);

	std::vector<PropertyPtr> algoOption = mReconstructer->getAlgoOptions();
	unsigned row = 0;
	for (;row < algoOption.size(); ++row)
	{
		sscCreateDataWidget(oneAlgoWidget, algoOption[row], oneAlgoLayout, row);
	}

	// Algorithms that apply an estimated time lag (option uid 'applyLag') replace the time calibration parameters
	if (mApplyLagProperty)
	{
		disconnect(mApplyLagProperty.get(), NULL, this, NULL);
	}
	mApplyLagProperty.reset();
	for (unsigned i = 0; i < algoOption.size(); ++i)
	{
		if (algoOption[i]->getUid() == "applyLag" && std::dynamic_pointer_cast<BoolProperty>(algoOption[i]))
		{
			mApplyLagProperty = algoOption[i];
			connect(mApplyLagProperty.get(), &Property::changed, this, &ReconstructionWidget::updateParameterStates);
		}
	}
	this->updateParameterStates();
	oneAlgoLayout->setRowStretch(row, 1); // Set stretch on last row

	mAlgoLayout->setCurrentWidget(oneAlgoWidget);
}

QString ReconstructionWidget::getCurrentPath()
{
	return QFileInfo(mReconstructer->getSelectedFilename()).dir().absolutePath();
}

void ReconstructionWidget::reconstruct()
{
	if (mRunner->isRunning())
	{
		mRunner->cancel();
	}
	else
	{
		this->storeOutputSelections();
		UsReconstructionRunner::Outputs outputs;
		outputs.mUsVolume = mUsVolumeCheckBox->isChecked();
		outputs.mAngioVolume = mAngioVolumeCheckBox->isChecked();
		outputs.mAngioMesh = mAngioMeshCheckBox->isChecked();
		std::vector<UsReconstructionRunner::ExtensionOutputs> extensions;
		for (unsigned i = 0; i < mExtensions.size(); ++i)
		{
			UsReconstructionRunner::ExtensionOutputs extension;
			extension.mExtension = mExtensions[i].mService;
			for (unsigned j = 0; j < mExtensions[i].mOutputs.size(); ++j)
			{
				if (mExtensions[i].mOutputs[j].second->isChecked())
				{
					extension.mOutputs << mExtensions[i].mOutputs[j].first;
				}
			}
			extensions.push_back(extension);
		}
		mResultsWidget->setData(std::vector<DataPtr>());
		mProgressBar->setValue(0);
		mStatusLabel->clear();
		mRunner->start(outputs, extensions);
	}
	this->updateButtons();
}

/** The US checkboxes, then those of the extensions. */
QList<QCheckBox*> ReconstructionWidget::getOutputCheckBoxes() const
{
	QList<QCheckBox*> retval = this->getUsCheckBoxes();
	for (unsigned i = 0; i < mExtensions.size(); ++i)
	{
		for (unsigned j = 0; j < mExtensions[i].mOutputs.size(); ++j)
		{
			retval << mExtensions[i].mOutputs[j].second;
		}
	}
	return retval;
}

void ReconstructionWidget::storeOutputSelections()
{
	QList<QCheckBox*> checkBoxes = this->getOutputCheckBoxes();
	for (int i = 0; i < checkBoxes.size(); ++i)
	{
		settings()->setValue("reconstruction/create/" + checkBoxes[i]->objectName(), checkBoxes[i]->isChecked());
	}
}

bool ReconstructionWidget::isAnyOutputSelected() const
{
	bool retval = false;
	QList<QCheckBox*> checkBoxes = this->getOutputCheckBoxes();
	for (int i = 0; i < checkBoxes.size(); ++i)
	{
		retval = retval || checkBoxes[i]->isChecked();
	}
	return retval;
}

/** The color an extension gives the data, or an invalid color. */
QColor ReconstructionWidget::getDataColor(DataPtr data) const
{
	QColor retval;
	for (unsigned i = 0; !retval.isValid() && i < mExtensions.size(); ++i)
	{
		retval = mExtensions[i].mService->getDataColor(data);
	}
	return retval;
}

void ReconstructionWidget::setDataColor(DataPtr data, QColor color)
{
	for (unsigned i = 0; i < mExtensions.size(); ++i)
	{
		if (mExtensions[i].mService->getDataColor(data).isValid())
		{
			mExtensions[i].mService->setDataColor(data, color);
		}
	}
}

void ReconstructionWidget::onExtensionAdded(UsReconstructionExtensionService* service)
{
	Extension extension;
	extension.mService = service;
	std::vector<UsReconstructionExtensionService::Output> outputs = service->getOutputs();
	for (unsigned i = 0; i < outputs.size(); ++i)
	{
		QCheckBox* checkBox = this->createOutputCheckBox(outputs[i].mUid, outputs[i].mName, outputs[i].mHelp, false);
		extension.mOutputs.push_back(std::make_pair(outputs[i].mUid, checkBox));
	}
	extension.mTab = service->createSettingsWidget(mTabs);
	mTabs->addTab(extension.mTab, service->getName());
	mExtensions.push_back(extension);
	this->updateButtons();
}

void ReconstructionWidget::onExtensionRemoved(UsReconstructionExtensionService* service)
{
	for (unsigned i = 0; i < mExtensions.size(); ++i)
	{
		if (mExtensions[i].mService == service)
		{
			for (unsigned j = 0; j < mExtensions[i].mOutputs.size(); ++j)
			{
				QCheckBox* checkBox = mExtensions[i].mOutputs[j].second;
				settings()->setValue("reconstruction/create/" + checkBox->objectName(), checkBox->isChecked());
				mOutputsLayout->removeWidget(mExtensions[i].mOutputs[j].second);
				mExtensions[i].mOutputs[j].second->hide();
				delete mExtensions[i].mOutputs[j].second;
			}
			mTabs->removeTab(mTabs->indexOf(mExtensions[i].mTab));
			delete mExtensions[i].mTab;
			mExtensions.erase(mExtensions.begin() + i);
			break;
		}
	}
	this->updateButtons();
}

void ReconstructionWidget::reload()
{
	this->selectData(mReconstructer->getSelectedFilename());
}

/**Called when data is loaded into reconstructer.
 *
 */
void ReconstructionWidget::inputDataSelected(QString mhdFileName)
{
	if (mReconstructer->getSelectedFilename().isEmpty())
	{
		return;
	}	

	mFileSelectWidget->setFilename(mhdFileName);
	this->updateButtons();
}

void ReconstructionWidget::selectData(QString filename)
{
	if (filename.isEmpty())
	{
		reportWarning("no file selected");
		return;
	}

	mReconstructer->selectData(filename);
}

/** Called when parameters in the reconstructer has changed
 *
 */
void ReconstructionWidget::paramsChangedSlot()
{
	Vector3D range = mReconstructer->getOutputVolumeParams().getExtent().range();

	QString extText =
		QString("%1,  %2,  %3").arg(range[0], 0, 'f', 1).arg(range[1], 0, 'f', 1).arg(range[2], 0, 'f', 1);
	mExtentLineEdit->setText(extText);

	mInputSpacingLineEdit->setText(QString("%1").arg(mReconstructer->getOutputVolumeParams().getInputSpacing(), 0, 'f', 4));

	OutputVolumeParams outputParams = mReconstructer->getOutputVolumeParams();
	mOutputSpacingLineEdit->setText(QString("%1").arg(outputParams.getSpacing(), 0, 'f', 4));
	Eigen::Array3i dim = outputParams.getDim();
	mDimXLineEdit->setText(QString::number(dim[0]));
	mDimYLineEdit->setText(QString::number(dim[1]));
	mDimZLineEdit->setText(QString::number(dim[2]));
}

void ReconstructionWidget::reconstructAboutToStartSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mReconstructer->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->attach(threads);
}

void ReconstructionWidget::reconstructStartedSlot()
{
	mUsReconstructing = true;
	this->updateButtons();
}

void ReconstructionWidget::reconstructFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mReconstructer->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->detach(threads);
	mUsReconstructing = this->isUsReconstructing();
	this->updateButtons();
}

/** Several reconstructions can run, e.g. one started after an acquisition. */
bool ReconstructionWidget::isUsReconstructing() const
{
	bool retval = false;
	std::set<cx::TimedAlgorithmPtr> threads = mReconstructer->getThreadedReconstruction();
	for (std::set<cx::TimedAlgorithmPtr>::iterator iter = threads.begin(); iter != threads.end(); ++iter)
	{
		retval = retval || (*iter && !(*iter)->isFinished());
	}
	return retval;
}

void ReconstructionWidget::runFinished()
{
	mResultsWidget->setData(mRunner->getCreatedData());
	this->updateButtons();
}

void ReconstructionWidget::progressChanged(int done, int total)
{
	mProgressBar->setMaximum(total);
	mProgressBar->setValue(done);
}

void ReconstructionWidget::updateButtons()
{
	bool running = mRunner->isRunning();
	bool canStart = !mReconstructer->getSelectedFilename().isEmpty() && this->isAnyOutputSelected();
	mReconstructButton->setText(running ? "Cancel" : "Reconstruct");
	mReconstructButton->setEnabled(running || (canStart && !mUsReconstructing));
	mProgressBar->setVisible(running && !mRunner->isReconstructingUs());
	mFileSelectWidget->setEnabled(!running && !mUsReconstructing);
	mOutputsLayout->parentWidget()->setEnabled(!running);
}


}//namespace

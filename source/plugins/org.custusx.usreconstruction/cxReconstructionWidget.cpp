/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxReconstructionWidget.h"

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

namespace cx
{

namespace
{
void setLayoutItemVisible(QLayoutItem* item, bool visible)
{
	if (!item)
		return;
	if (QWidget* widget = item->widget())
	{
		widget->setVisible(visible);
	}
	else if (QLayout* layout = item->layout())
	{
		for (int i = 0; i < layout->count(); ++i)
			setLayoutItemVisible(layout->itemAt(i), visible);
	}
}

/** Show or hide all widgets (label and input) in a row of a grid layout */
void setGridRowVisible(QGridLayout* layout, int row, bool visible)
{
	for (int column = 0; column < layout->columnCount(); ++column)
		setLayoutItemVisible(layout->itemAtPosition(row, column), visible);
}
}

ReconstructionWidget::ReconstructionWidget(QWidget* parent, UsReconstructionServicePtr reconstructer, PatientModelServicePtr patientModelService) :
	BaseWidget(parent, "us_reconstruction", "US Reconstruction"),
	mReconstructer(reconstructer),
	mFileSelectWidget( new FileSelectWidget(this)),
	mOptionsLayout(NULL)
{
	this->setToolTip("Reconstruct 3D US data from an acquired 2D sequence");
	connect(mReconstructer.get(), &UsReconstructionService::reconstructAboutToStart, this, &ReconstructionWidget::reconstructAboutToStartSlot);
	connect(mReconstructer.get(), &UsReconstructionService::reconstructStarted, this, &ReconstructionWidget::reconstructStartedSlot);
	connect(mReconstructer.get(), &UsReconstructionService::reconstructFinished, this, &ReconstructionWidget::reconstructFinishedSlot);

	connect(mReconstructer.get(), &UsReconstructionService::paramsChanged, this, &ReconstructionWidget::paramsChangedSlot);
	connect(mReconstructer.get(), &UsReconstructionService::inputDataSelected, this, &ReconstructionWidget::inputDataSelected);
	connect(mReconstructer.get(), &UsReconstructionService::algorithmChanged, this, &ReconstructionWidget::repopulateAlgorithmGroup);


	QVBoxLayout* topLayout = new QVBoxLayout(this);

	connect(mFileSelectWidget, &FileSelectWidget::fileSelected, this, &ReconstructionWidget::selectData);
	mFileSelectWidget->setNameFilter(QStringList() << "*.fts");
	// The Select data dialog starts in the US_Acq folder of the active patient
	mFileSelectWidget->setDialogFolderProvider([patientModelService]() -> QString
	{
		QString folder;
		if (patientModelService && patientModelService->isPatientValid())
		{
			folder = patientModelService->getActivePatientFolder() + "/US_Acq";
		}
		return folder;
	});
	connect(mReconstructer.get(), &UsReconstructionService::newInputDataAvailable, mFileSelectWidget, &FileSelectWidget::refresh);
	connect(mReconstructer.get(), &UsReconstructionService::newInputDataPath, this, &ReconstructionWidget::updateFileSelectorPath);

	QHBoxLayout* extentLayout = new QHBoxLayout;
	mExtentLineEdit = new QLineEdit(this);
	mExtentLineEdit->setReadOnly(true);
	extentLayout->addWidget(new QLabel("Extent", this));
	extentLayout->addWidget(mExtentLineEdit);

	mInputSpacingLineEdit = new QLineEdit(this);
	mInputSpacingLineEdit->setReadOnly(true);
	QLabel* inputSpacingLabel = new QLabel("Spacing In", this);

	mReconstructButton = new QPushButton("Reconstruct", this);
	connect(mReconstructButton, &QPushButton::clicked, this, &ReconstructionWidget::reconstruct);

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;

	QGridLayout* sizesLayout = new QGridLayout;
	QWidget* maxVolSizeWidget = sscCreateDataWidget(this, mReconstructer->getParam("Volume Size"));
	sizesLayout->addWidget(inputSpacingLabel, 0, 0);
	sizesLayout->addWidget(mInputSpacingLineEdit, 0, 1);
	sizesLayout->addLayout(extentLayout, 0, 2);
	mOutputSpacingLineEdit = this->createIndicator();
	sizesLayout->addWidget(new QLabel("Spacing Out", this), 1, 0);
	sizesLayout->addWidget(mOutputSpacingLineEdit, 1, 1);
	sizesLayout->addWidget(maxVolSizeWidget, 1, 2);

	QHBoxLayout* runLayout = new QHBoxLayout;
	topLayout->addLayout(runLayout);
	runLayout->addWidget(mReconstructButton);
	this->createAction(this,
	      QIcon(":/icons/open_icon_library/system-run-5.png"),
	      "Details", "Show Advanced Settings",
	      SLOT(toggleDetailsSlot()),
	      runLayout);

	topLayout->addWidget(mFileSelectWidget);
	topLayout->addLayout(sizesLayout);

	QWidget* presetTFWidget = sscCreateDataWidget(this, mReconstructer->getParam("Preset"));
	topLayout->addWidget(presetTFWidget);
	topLayout->addWidget(sscCreateDataWidget(this, mReconstructer->getParam("Reduce mask (% in 1D)")));

	mDimXLineEdit = this->createIndicator();
	mDimYLineEdit = this->createIndicator();
	mDimZLineEdit = this->createIndicator();
	QHBoxLayout* outputVolDimLayout = new QHBoxLayout;
	outputVolDimLayout->addWidget(new QLabel("X", this));
	outputVolDimLayout->addWidget(mDimXLineEdit);
	outputVolDimLayout->addWidget(new QLabel("Y", this));
	outputVolDimLayout->addWidget(mDimYLineEdit);
	outputVolDimLayout->addWidget(new QLabel("Z", this));
	outputVolDimLayout->addWidget(mDimZLineEdit);
	topLayout->addLayout(outputVolDimLayout);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("reconstruction/guiShowDetails").toBool());
	topLayout->addWidget(mOptionsWidget);

	topLayout->addStretch();
	topLayout->addWidget(mTimedAlgorithmProgressBar);

	this->paramsChangedSlot();
}

QLineEdit* ReconstructionWidget::createIndicator()
{
	QLineEdit* retval = new QLineEdit(this);
	retval->setReadOnly(true);
	return retval;
}

void ReconstructionWidget::updateFileSelectorPath(QString path)
{
	mFileSelectWidget->setPath(path);
	mFileSelectWidget->refresh();
}

void ReconstructionWidget::toggleDetailsSlot()
{
  mOptionsWidget->setVisible(!mOptionsWidget->isVisible());
  settings()->setValue("reconstruction/guiShowDetails", mOptionsWidget->isVisible());
}

QWidget* ReconstructionWidget::createOptionsWidget()
{
	QWidget* retval = new QWidget(this);
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);
	mOptionsLayout = layout;

	int line = 0;

	layout->addWidget(this->createHorizontalLine(), line, 0, 1, 2);
	++line;

	mAlgorithmGroup = new QFrame(this);
	mAlgorithmGroup->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	mAlgorithmGroup->setFocusPolicy(Qt::StrongFocus); // needed for help system: focus is used to display help text
	mAlgorithmGroup->setSizePolicy(mAlgorithmGroup->sizePolicy().horizontalPolicy(),QSizePolicy::Fixed);

	QVBoxLayout* algoOuterLayout = new QVBoxLayout(mAlgorithmGroup);
	//algoOuterLayout->setMargin(0);
	QWidget* algorithmWidget = sscCreateDataWidget(this, mReconstructer->getParam("Algorithm"));
	algoOuterLayout->addWidget(algorithmWidget);
	mAlgoLayout = new QStackedLayout;
	this->repopulateAlgorithmGroup();
	mAlgoLayout->setMargin(0);
	algoOuterLayout->addLayout(mAlgoLayout);
	//algoOuterLayout->addStretch();
	layout->addWidget(mAlgorithmGroup, line, 0, 1, 2);
	++line;

	sscCreateDataWidget(this, mReconstructer->getParam("Angio data"), layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParam("Dual Angio"), layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParam("Orientation"), layout, line++);
	layout->addWidget(this->createHorizontalLine(), line++, 0, 1, 2);

	sscCreateDataWidget(this, mReconstructer->getParam("Align timestamps"), layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParam("Extra Temporal Calib"), layout, line++);
//    sscCreateDataWidget(this, mReconstructer->getParam("Position Thinning"), layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParam("Position Filter"), layout, line++);
	mMovingAverageRows.clear();
	mHoltRows.clear();
	mMovingAverageRows.push_back(line);
	sscCreateDataWidget(this, mReconstructer->getParam("Position Filter Strength"), layout, line++);
	QStringList holtParameters = QStringList() << "Holt alpha" << "Holt beta" << "Holt both directions" << "Holt add trend";
	for (int i = 0; i < holtParameters.size(); ++i)
	{
		mHoltRows.push_back(line);
		sscCreateDataWidget(this, mReconstructer->getParam(holtParameters[i]), layout, line++);
	}
	sscCreateDataWidget(this, mReconstructer->getParam("Save processed positions"), layout, line++);

	// The parameters that are shown or enabled depend on other parameters
	QStringList controllingParameters = QStringList() << "Position Filter" << "Align timestamps";
	for (int i = 0; i < controllingParameters.size(); ++i)
		connect(mReconstructer->getParam(controllingParameters[i]).get(), &Property::changed, this, &ReconstructionWidget::updateParameterStates);
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
			setGridRowVisible(mOptionsLayout, mMovingAverageRows[i], filter == "Moving average");
		for (unsigned i = 0; i < mHoltRows.size(); ++i)
			setGridRowVisible(mOptionsLayout, mHoltRows[i], filter == "Holt");
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
		disconnect(mApplyLagProperty.get(), NULL, this, NULL);
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
	mReconstructer->startReconstruction();
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
	mReconstructButton->setEnabled(false);
}

void ReconstructionWidget::reconstructFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mReconstructer->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->detach(threads);
		mReconstructButton->setEnabled(true);
}


}//namespace
